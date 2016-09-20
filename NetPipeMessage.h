#pragma once

#include <memory>
#include <functional>

#include "NetSerialize.h"
#include "NetDeserialize.h"

class NetBitWriter;
class NetBitReader;

template <class BaseClass, class Sink>
class NetPipeMessage
{
public:

  template <class... Callbacks>
  NetPipeMessage(const Sink & sink, Callbacks &&... callbacks)
    : m_Sink(sink)
  {
    auto & type_db = BaseClass::__s_TypeDatabase;
    m_Callbacks = std::make_unique<std::function<void(NetBitReader &)>[]>(type_db.GetNumTypes());

    RegisterCallbackPack(std::forward<Callbacks>(callbacks)...);
  }

  template <class DataType>
  void SendMessage(const DataType & data)
  {
    auto & type_db = BaseClass::__s_TypeDatabase;
    auto class_id = type_db.GetClassId<DataType>();

    NetBitWriter writer = m_Sink.CreateWriter();

    writer.WriteBits(class_id, GetRequiredBits(type_db.GetNumTypes()));
    NetSerializeValue(data, writer);

    m_Sink(writer);
  }

  void GotMessage(NetBitReader & reader)
  {
    auto & type_db = BaseClass::__s_TypeDatabase;
    auto class_id = reader.ReadUBits(GetRequiredBits(BaseClass::__s_TypeDatabase.GetNumTypes()));

    if (m_Callbacks[class_id])
    {
      m_Callbacks[class_id](reader);
    }
  }

protected:

  template <class First, class... Args>
  void RegisterCallbackPack(First && first, Args &&... args)
  {
    RegisterCallbackDeduce(first);
    RegisterCallbackPack(args...);
  }

  void RegisterCallbackPack()
  {

  }

  template <class DataType, class CallbackType>
  void RegisterCallback(CallbackType && callback)
  {
    auto & type_db = BaseClass::__s_TypeDatabase;
    auto class_id = type_db.GetClassId<DataType>();

    auto deserialize_cb = [=](NetBitReader & reader)
    {
      DataType dt;
      NetDeserializeValue(dt, reader);
      callback(dt);
    };

    m_Callbacks[class_id] = deserialize_cb;
  }

  template <typename T>
  struct func_traits : public func_traits<decltype(&T::operator())> {};

  template <typename C, typename Ret, typename... Args>
  struct func_traits<Ret(C::*)(Args...) const> {
    using result_type = Ret;

    template <std::size_t i>
    struct arg {
      using type = typename std::tuple_element<i, std::tuple<Args...>>::type;
    };
  };

  template <class CallbackType>
  void RegisterCallbackDeduce(CallbackType && callback)
  {
    using traits = func_traits<typename std::decay<CallbackType>::type>;
    using arg0_t = typename traits::template arg<0>::type; // First arg type.

    RegisterCallback<std::decay_t<arg0_t>, CallbackType>(callback);
  }

private:
  std::unique_ptr<std::function<void(NetBitReader &)>[]> m_Callbacks;
  Sink m_Sink;
};

