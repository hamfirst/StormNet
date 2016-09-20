#pragma once

#include <vector>
#include <typeinfo>

#define NET_INVALID_TYPE_HASH 0
#define NET_INVALID_CLASS_ID 0x7FFFFFFF

#define NET_DECLARE_BASE_TYPE \
public:\
  static NetTypeDatabase __s_TypeDatabase;

#define NET_REGISTER_TYPE_IMPL(ClassName, ParentTypeHash, TypeDb) \
  class _s_reg##ClassName { public: _s_reg##ClassName() { \
    static NetTypeRegistrationInfo reg; \
    reg.m_ClassName = #ClassName; \
    reg.m_TypeInfo.m_TypeIdHash = typeid(ClassName).hash_code(); \
    reg.m_TypeInfo.m_ParentIdHash = ParentTypeHash; \
    reg.m_TypeInfo.m_ParentClassId = NET_INVALID_CLASS_ID; \
    reg.m_TypeInfo.m_HeapCreate = []() -> void * { return new ClassName(); }; \
    reg.m_TypeInfo.m_HeapDestroy = [](void * ptr) { delete static_cast<ClassName *>(ptr); }; \
    reg.m_TypeInfo.m_Serialize = [](const void * val, NetBitWriter & writer) { NetSerializeValue<ClassName>(*static_cast<const ClassName *>(val), writer); }; \
    reg.m_TypeInfo.m_SerializeDelta = [](const void * val, const void * compare, NetBitWriter & writer) { return NetSerializeValueDelta<ClassName>(*static_cast<const ClassName *>(val), *static_cast<const ClassName *>(compare), writer); }; \
    reg.m_TypeInfo.m_Deserialize = [](void * val, NetBitReader & reader) { NetDeserializeValue<ClassName>(*static_cast<ClassName *>(val), reader); }; \
    reg.m_TypeInfo.m_DeserializeDelta = [](void * val, NetBitReader & reader) { NetDeserializeValueDelta<ClassName>(*static_cast<ClassName *>(val), reader); }; \
    reg.m_TypeInfo.m_Compare = [](const void * val1, const void * val2) { return StormReflCompare(*static_cast<const ClassName *>(val1), *static_cast<const ClassName *>(val2)); }; \
    reg.m_TypeInfo.m_Copy = [](void * val1, void * val2) { (*static_cast<ClassName *>(val1)) = (*static_cast<ClassName *>(val2)); }; \
    TypeDb.AddClass(&reg); \
   } }; static _s_reg##ClassName _s_regInst##ClassName;

#define NET_REGISTER_BASE_TYPE(ClassName) \
  NetTypeDatabase ClassName::__s_TypeDatabase; \
  NET_REGISTER_TYPE_IMPL(ClassName, NET_INVALID_TYPE_HASH, ClassName::__s_TypeDatabase)

#define NET_REGISTER_TYPE(ClassName, BaseClass) \
  NET_REGISTER_TYPE_IMPL(ClassName, typeid(BaseClass).hash_code(), ClassName::__s_TypeDatabase)

class NetBitWriter;
class NetBitReader;

struct NetTypeInfo
{
  std::size_t m_TypeIdHash;
  std::size_t m_ParentIdHash;

  std::size_t m_ParentClassId;

  void * (*m_HeapCreate)();
  void (*m_HeapDestroy)(void * val);
  void (*m_Serialize)(const void * val, NetBitWriter & writer);
  bool (*m_SerializeDelta)(const void * val, const void * compare, NetBitWriter & writer);
  void (*m_Deserialize)(void * val, NetBitReader & reader);
  void (*m_DeserializeDelta)(void * val, NetBitReader & reader);
  bool (*m_Compare)(const void * val1, const void * val2);
  void (*m_Copy)(void * val1, void * val2);
};

struct NetTypeRegistrationInfo
{
  const char * m_ClassName;
  NetTypeInfo m_TypeInfo;

  NetTypeRegistrationInfo * m_Next;
};

class NetTypeDatabase
{
public:
  NetTypeDatabase() = default;
  NetTypeDatabase(const NetTypeDatabase & rhs) = delete;
  NetTypeDatabase(NetTypeDatabase && rhs) = delete;

  NetTypeDatabase & operator = (const NetTypeDatabase & rhs) = delete;
  NetTypeDatabase & operator = (NetTypeDatabase && rhs) = delete;

  void AddClass(NetTypeRegistrationInfo * type_info);
  std::size_t GetDefaultClassId();
  std::size_t GetClassId(std::size_t type_hash);

  std::size_t GetNumTypes();

  const NetTypeInfo & GetDefaultTypeInfo();
  const NetTypeInfo & GetTypeInfo(std::size_t class_id);

  bool InheritsFrom(std::size_t class_id, std::size_t base_class_id);

  template <class T>
  std::size_t GetClassId()
  {
    return GetClassId(typeid(T).hash_code());
  }

  template <class T>
  const NetTypeInfo & GetTypeInfo()
  {
    return GetTypeInfo(typeid(T).hash_code());
  }

private:

  void FinalizeList();

  NetTypeRegistrationInfo * m_TypeInfoList = nullptr;
  bool m_Initialized = false;

  NetTypeInfo m_DefaultTypeInfo;
  std::size_t m_DefaultTypeClassId;

  std::vector<NetTypeInfo> m_TypeDatabase;
};

