#pragma once

#include <vector>
#include <typeinfo>
#include <cstddef>

#include <hash/Hash.h>

#define NET_INVALID_TYPE_HASH 0
#define NET_INVALID_CLASS_ID 0x7FFFFFFF

#define NET_DECLARE_BASE_TYPE \
public:\
  static NetTypeDatabase __s_TypeDatabase;\
  static NetTypeRegistrationInfo * __s_TypeInfoList;

#define NET_REGISTER_TYPE_IMPL(ClassName, ParentTypeHash, TypeDb, TypeList) \
  class _s_reg##ClassName { public: _s_reg##ClassName() { \
    static NetTypeRegistrationInfo reg; \
    reg.m_TypeInfo.m_ClassName = #ClassName; \
    reg.m_TypeInfo.m_Abstract = false; \
    reg.m_TypeInfo.m_TypeNameHash = crc32(#ClassName); \
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
    reg.m_TypeInfo.m_Copy = [](void * val1, const void * val2) { (*static_cast<ClassName *>(val1)) = (*static_cast<const ClassName *>(val2)); }; \
    TypeDb.AddClass(&reg, TypeList); \
   } }; static _s_reg##ClassName _s_regInst##ClassName;

#define NET_REGISTER_ABSTRACT_TYPE_IMPL(ClassName, ParentTypeHash, TypeDb, TypeList) \
  class _s_reg##ClassName { public: _s_reg##ClassName() { \
    static NetTypeRegistrationInfo reg; \
    reg.m_TypeInfo.m_ClassName = #ClassName; \
    reg.m_TypeInfo.m_Abstract = true; \
    reg.m_TypeInfo.m_TypeNameHash = crc32(#ClassName); \
    reg.m_TypeInfo.m_TypeIdHash = typeid(ClassName).hash_code(); \
    reg.m_TypeInfo.m_ParentIdHash = ParentTypeHash; \
    reg.m_TypeInfo.m_ParentClassId = NET_INVALID_CLASS_ID; \
    reg.m_TypeInfo.m_HeapCreate = []() -> void * { return nullptr; }; \
    reg.m_TypeInfo.m_HeapDestroy = [](void * ptr) { }; \
    reg.m_TypeInfo.m_Serialize = [](const void * val, NetBitWriter & writer) { NetSerializeValue<ClassName>(*static_cast<const ClassName *>(val), writer); }; \
    reg.m_TypeInfo.m_SerializeDelta = [](const void * val, const void * compare, NetBitWriter & writer) { return NetSerializeValueDelta<ClassName>(*static_cast<const ClassName *>(val), *static_cast<const ClassName *>(compare), writer); }; \
    reg.m_TypeInfo.m_Deserialize = [](void * val, NetBitReader & reader) { NetDeserializeValue<ClassName>(*static_cast<ClassName *>(val), reader); }; \
    reg.m_TypeInfo.m_DeserializeDelta = [](void * val, NetBitReader & reader) { NetDeserializeValueDelta<ClassName>(*static_cast<ClassName *>(val), reader); }; \
    reg.m_TypeInfo.m_Compare = [](const void * val1, const void * val2) { return StormReflCompare(*static_cast<const ClassName *>(val1), *static_cast<const ClassName *>(val2)); }; \
    reg.m_TypeInfo.m_Copy = [](void * val1, const void * val2) { (*static_cast<ClassName *>(val1)) = (*static_cast<const ClassName *>(val2)); }; \
    TypeDb.AddClass(&reg, TypeList); \
   } }; static _s_reg##ClassName _s_regInst##ClassName;

#define NET_REGISTER_BASE_TYPE(ClassName) \
  NetTypeDatabase ClassName::__s_TypeDatabase(ClassName::__s_TypeInfoList); \
  NetTypeRegistrationInfo * ClassName::__s_TypeInfoList; \
  NET_REGISTER_TYPE_IMPL(ClassName, NET_INVALID_TYPE_HASH, ClassName::__s_TypeDatabase, ClassName::__s_TypeInfoList)

#define NET_REGISTER_BASE_TYPE_ABSTRACT(ClassName) \
  NetTypeDatabase ClassName::__s_TypeDatabase(ClassName::__s_TypeInfoList); \
  NetTypeRegistrationInfo * ClassName::__s_TypeInfoList; \
  NET_REGISTER_ABSTRACT_TYPE_IMPL(ClassName, NET_INVALID_TYPE_HASH, ClassName::__s_TypeDatabase, ClassName::__s_TypeInfoList)

#define NET_REGISTER_TYPE(ClassName, BaseClass) \
  NET_REGISTER_TYPE_IMPL(ClassName, typeid(BaseClass).hash_code(), ClassName::__s_TypeDatabase, ClassName::__s_TypeInfoList)

#define NET_REGISTER_TYPE_ABSTRACT(ClassName, BaseClass) \
  NET_REGISTER_ABSTRACT_TYPE_IMPL(ClassName, typeid(BaseClass).hash_code(), ClassName::__s_TypeDatabase, ClassName::__s_TypeInfoList)

class NetBitWriter;
class NetBitReader;

struct NetTypeInfo
{
  const char * m_ClassName;
  uint32_t m_TypeNameHash;
  std::size_t m_TypeIdHash;
  std::size_t m_ParentIdHash;

  std::size_t m_ParentClassId;
  std::size_t m_ClassId;

  bool m_Abstract;

  void * (*m_HeapCreate)();
  void (*m_HeapDestroy)(void * val);
  void (*m_Serialize)(const void * val, NetBitWriter & writer);
  bool (*m_SerializeDelta)(const void * val, const void * compare, NetBitWriter & writer);
  void (*m_Deserialize)(void * val, NetBitReader & reader);
  void (*m_DeserializeDelta)(void * val, NetBitReader & reader);
  bool (*m_Compare)(const void * val1, const void * val2);
  void (*m_Copy)(void * val1, const void * val2);
};

struct NetTypeRegistrationInfo
{
  NetTypeInfo m_TypeInfo;

  NetTypeRegistrationInfo * m_Next;
};

class NetTypeDatabase
{
public:
  NetTypeDatabase(NetTypeRegistrationInfo * & type_info_list);
  NetTypeDatabase(const NetTypeDatabase & rhs) = delete;
  NetTypeDatabase(NetTypeDatabase && rhs) = delete;

  NetTypeDatabase & operator = (const NetTypeDatabase & rhs) = delete;
  NetTypeDatabase & operator = (NetTypeDatabase && rhs) = delete;

  void AddClass(NetTypeRegistrationInfo * type_info, NetTypeRegistrationInfo * & type_list);
  std::size_t GetDefaultClassId();
  std::size_t GetClassId(std::size_t type_hash);

  void SetDefaultTypeNameHash(std::size_t type_hash);

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
    return GetTypeInfo(GetClassId<T>());
  }

private:

  void FinalizeList();

private:
  NetTypeRegistrationInfo * & m_TypeInfoList;
  bool m_Initialized;

  NetTypeInfo m_DefaultTypeInfo;
  std::size_t m_DefaultTypeClassId;
  std::size_t m_DefaultTypeNameHash;

  std::vector<NetTypeInfo> m_TypeDatabase;
};

