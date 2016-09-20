#pragma once

template <class DataInst, int Size>
class NetStateStore
{
public:

  DataInst & Get(int index)
  {
    return m_Store[index];
  }

  const DataInst & Get(int index)
  {
    return m_Store[index];
  }

private:
  DataInst m_Store[Size];
};

