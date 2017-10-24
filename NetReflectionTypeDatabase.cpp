
#include "NetReflectionTypeDatabase.h"
#include "NetException.h"

#include <algorithm>
#include <cstring>

void NetTypeDatabase::AddClass(NetTypeRegistrationInfo * type_info)
{
  if (m_Initialized)
  {
    NET_THROW(std::logic_error("Attempting to modify type database after it has been initialized"));
  }

  if (m_TypeInfoList == nullptr)
  {
    m_DefaultTypeInfo = type_info->m_TypeInfo;
  }

  type_info->m_Next = m_TypeInfoList;
  m_TypeInfoList = type_info;
}

std::size_t NetTypeDatabase::GetDefaultClassId()
{
  return m_DefaultTypeClassId;
}

std::size_t NetTypeDatabase::GetClassId(std::size_t type_hash)
{
  if (m_Initialized == false)
  {
    FinalizeList();
  }

  for (std::size_t index = 0; index < m_TypeDatabase.size(); index++)
  {
    if (m_TypeDatabase[index].m_TypeIdHash == type_hash)
    {
      return index;
    }
  }

  NET_THROW_OR(std::out_of_range("Could not find class"), return -1);
}

std::size_t NetTypeDatabase::GetNumTypes()
{
  if (m_Initialized == false)
  {
    FinalizeList();
  }

  return m_TypeDatabase.size();
}

const NetTypeInfo & NetTypeDatabase::GetDefaultTypeInfo()
{
  return m_DefaultTypeInfo;
}

const NetTypeInfo & NetTypeDatabase::GetTypeInfo(std::size_t class_id)
{
  if (m_Initialized == false)
  {
    FinalizeList();
  }

  return m_TypeDatabase[class_id];
}


bool NetTypeDatabase::InheritsFrom(std::size_t class_id, std::size_t base_class_id)
{
  while (class_id != NET_INVALID_CLASS_ID)
  {
    if (class_id == base_class_id)
    {
      return true;
    }

    class_id = m_TypeDatabase[class_id].m_ParentClassId;
  }

  return false;
}

void NetTypeDatabase::FinalizeList()
{
  std::vector<NetTypeRegistrationInfo *> all_reg_info;

  for (NetTypeRegistrationInfo * type_info = m_TypeInfoList; type_info != nullptr; type_info = type_info->m_Next)
  {
    all_reg_info.push_back(type_info);
  }

  auto sort_func = [](const NetTypeRegistrationInfo * a, const NetTypeRegistrationInfo * b)
  {
    return strcmp(a->m_ClassName, b->m_ClassName) < 0;
  };

  std::sort(all_reg_info.begin(), all_reg_info.end(), sort_func);

  m_TypeDatabase.clear();
  std::size_t class_id = 0;

  for (auto & info : all_reg_info)
  {
    info->m_TypeInfo.m_ClassId = class_id;
    class_id++;

    if (info->m_TypeInfo.m_ParentIdHash != NET_INVALID_TYPE_HASH)
    {
      for(size_t base_index = 0; base_index < all_reg_info.size(); base_index++)
      {
        auto & base = all_reg_info[base_index];
        if (base->m_TypeInfo.m_TypeIdHash == info->m_TypeInfo.m_ParentIdHash)
        {
          info->m_TypeInfo.m_ParentClassId = base_index;
          break;
        }
      }
    }

    if (info->m_TypeInfo.m_TypeIdHash == m_DefaultTypeInfo.m_TypeIdHash)
    {
      m_DefaultTypeClassId = m_TypeDatabase.size();
    }

    m_TypeDatabase.emplace_back(info->m_TypeInfo);
  }

  m_TypeInfoList = nullptr;
  m_Initialized = true;
}

