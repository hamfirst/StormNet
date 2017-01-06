#pragma once

#include <memory>

template <class DataInst, int Size>
class NetStateStore
{
public:

  void StoreState(const std::shared_ptr<DataInst> & state, int index)
  {
    m_Store[index] = state;
  }

  DataInst * Get(int index)
  {
    return m_Store[index].get();
  }

  const DataInst * Get(int index) const
  {
    return m_Store[index].get();
  }

private:
  std::shared_ptr<DataInst> m_Store[Size];
};

