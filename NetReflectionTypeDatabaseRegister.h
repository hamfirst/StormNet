#pragma once

#include <type_traits>

#include <StormRefl/StormReflMetaInfoBase.h>

#include "NetSerialize.h"
#include "NetSerializeDelta.h"
#include "NetDeserialize.h"
#include "NetDeserializeDelta.h"
#include "NetReflectionTypeDatabase.h"


template <typename RegType>
void NetInitializeTypeRegistration(NetTypeRegistrationInfo & reg)
{
  reg.m_ClassName = StormReflTypeInfo<RegType>::GetName();
  reg.m_TypeInfo.m_Abstract = false;
  reg.m_TypeInfo.m_TypeIdHash = typeid(RegType).hash_code();
  reg.m_TypeInfo.m_ParentIdHash = NET_INVALID_TYPE_HASH;
  reg.m_TypeInfo.m_ParentClassId = NET_INVALID_CLASS_ID;
  reg.m_TypeInfo.m_HeapCreate = []() -> void * { return new RegType(); };
  reg.m_TypeInfo.m_HeapDestroy = [](void * ptr) { delete static_cast<RegType *>(ptr); };
  reg.m_TypeInfo.m_Serialize = [](const void * val, NetBitWriter & writer) { NetSerializeValue<RegType>(*static_cast<const RegType *>(val), writer); };
  reg.m_TypeInfo.m_SerializeDelta = [](const void * val, const void * compare, NetBitWriter & writer) { return NetSerializeValueDelta<RegType>(*static_cast<const RegType *>(val), *static_cast<const RegType *>(compare), writer); };
  reg.m_TypeInfo.m_Deserialize = [](void * val, NetBitReader & reader) { NetDeserializeValue<RegType>(*static_cast<RegType *>(val), reader); };
  reg.m_TypeInfo.m_DeserializeDelta = [](void * val, NetBitReader & reader) { NetDeserializeValueDelta<RegType>(*static_cast<RegType *>(val), reader); };
  reg.m_TypeInfo.m_Compare = [](const void * val1, const void * val2) { return StormReflCompare(*static_cast<const RegType *>(val1), *static_cast<const RegType *>(val2)); };
  reg.m_TypeInfo.m_Copy = [](void * val1, const void * val2) { (*static_cast<RegType *>(val1)) = (*static_cast<const RegType *>(val2)); };
}

template <typename ChildType, typename BaseType>
void NetRegisterPolymorphicType()
{
  static NetTypeRegistrationInfo reg;

  NetInitializeTypeRegistration<ChildType>(reg);
  reg.m_TypeInfo.m_ParentIdHash = typeid(BaseType).hash_code();
  BaseType::__s_TypeDatabase.AddClass(&reg);
}

template <bool IsChild, typename ChildType, typename BaseType>
struct NetRegisterChildType
{
  static void Process()
  {
    static NetTypeRegistrationInfo reg;
    NetInitializeTypeRegistration<ChildType>(reg);
    reg.m_TypeInfo.m_ParentIdHash = typeid(BaseType).hash_code();
    BaseType::__s_TypeDatabase.AddClass(&reg, BaseType::__s_TypeInfoList);
  }
};

template <typename ChildType, typename BaseType>
struct NetRegisterChildType<false, ChildType, BaseType>
{
  static void Process()
  {

  }
};

template <int I, typename BaseType, typename FileMetaType>
struct NetCheckChildType
{
  using MyType = typename FileMetaType::template type_info<I>::type;
  using MyBaseType = typename StormReflTypeInfo<MyType>::MyBase;

  static constexpr bool IsBase = std::is_same<BaseType, MyBaseType>::value;

  static void Process()
  {
    NetRegisterChildType<IsBase, MyType, BaseType>::Process();
    NetCheckChildType<I - 1, BaseType, FileMetaType>::Process();
  }
};

template <typename BaseType, typename FileMetaType>
struct NetCheckChildType<-1, BaseType, FileMetaType>
{
  static void Process()
  {

  }
};

template <bool IsBaseType, typename BaseType, typename FileMetaType>
struct NetRegisterBaseType
{
  static void Process()
  {
    static NetTypeRegistrationInfo reg;
    NetInitializeTypeRegistration<BaseType>(reg);
    BaseType::__s_TypeDatabase.AddClass(&reg, BaseType::__s_TypeInfoList);

    NetCheckChildType<FileMetaType::types_n - 1, BaseType, FileMetaType>::Process();
  }
};

template <typename BaseType, typename FileMetaType>
struct NetRegisterBaseType<false, BaseType, FileMetaType>
{
  static void Process()
  {

  }
};

template<typename T, typename Enable = void> struct NetCheckHasTypeDatabase {
  struct Fallback { NetTypeDatabase __s_TypeDatabase; };
  struct Derived : T, Fallback { };

  template<typename C, C> struct ChT;

  template<typename C> static char(&f(ChT<NetTypeDatabase Fallback::*, &C::__s_TypeDatabase>*))[1];
  template<typename C> static char(&f(...))[2];

  static const bool value = sizeof(f<Derived>(0)) == 2;
};

template <int I, typename FileMetaType>
struct NetCheckType
{
  using MyType = typename FileMetaType::template type_info<I>::type;

  static constexpr bool HasNoBase = std::is_same<typename StormReflTypeInfo<MyType>::MyBase, void>::value;
  static constexpr bool HasTypeDb = NetCheckHasTypeDatabase<MyType>::value;

  static void Process()
  {
    NetRegisterBaseType<HasNoBase && HasTypeDb, MyType, FileMetaType>::Process();
    NetCheckType<I - 1, FileMetaType>::Process();
  }
};

template <typename FileMetaType>
struct NetCheckType<-1, FileMetaType>
{
  static void Process()
  {

  }
};

template <typename FileMetaType>
struct NetRegisterAllTypes
{
  NetRegisterAllTypes()
  {
    NetCheckType<FileMetaType::types_n - 1, FileMetaType>::Process();
  }
};


#define NET_REGISTER_ALL_TYPES_FOR_FILE(FileName) \
  struct _s_reg_##FileName \
  { \
    _s_reg_##FileName() \
    { \
      NetRegisterAllTypes<StormReflFileInfo::FileName> reg; \
    } \
  }; static _s_reg_##FileName _s_reg_##FileName_Inst; \

#define NET_BASE_TYPE(BaseType) \
  NetTypeDatabase BaseType::__s_TypeDatabase(BaseType::__s_TypeInfoList); \
  NetTypeRegistrationInfo * BaseType::__s_TypeInfoList; \

#define NET_SET_DEFAULT_TYPE(BaseType, DefaultType) \
   struct _s_reg_Default_##BaseType \
  { \
    _s_reg_Default_##BaseType() \
    { \
      BaseType::__s_TypeDatabase.SetDefaultTypeNameHash(crc32(#DefaultType)); \
    } \
  }; static _s_reg_Default_##BaseType _s_reg_Default_##BaseType##_Inst; \

