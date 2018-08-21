#pragma once

#include <type_traits>
#include <tuple>

namespace NetMetaUtil
{
  template <class F, class Tuple, std::size_t... I>
  constexpr decltype(auto) ApplyImpl(F &&f, Tuple &&t, std::index_sequence<I...>)
  {
    return f(std::get<I>(std::forward<Tuple>(t))...);
  }

  template <class F, class Tuple>
  constexpr decltype(auto) Apply(F &&f, Tuple &&t)
  {
    return ApplyImpl(
      std::forward<F>(f), std::forward<Tuple>(t),
      std::make_index_sequence<std::tuple_size<std::decay_t<Tuple>>::value>{});
  }



  template <class T, class ... Types>
  struct NetVariantUtil
  {
    static constexpr int GetTypeIndex(int i = 0)
    {
      return -1;
    }
  };

  template <class T, class Test, class ... Types>
  struct NetVariantUtil<T, Test, Types...>
  {
    static constexpr int GetTypeIndex(int i = 0)
    {
      return std::template is_same<T, Test>::value ? i : NetVariantUtil<T, Types...>::GetTypeIndex(i + 1);
    }
  };

  template <class T, class ... Types>
  constexpr int GetTypeIndex()
  {
    return NetVariantUtil<T, Types...>::GetTypeIndex();
  }

  template <int Index>
  struct NetVariantVisitor
  {
    template <typename Visitor, typename Tuple>
    static void Process(Visitor && visitor, Tuple && tuple)
    {
      constexpr auto Size = std::template tuple_size<std::template decay_t<Tuple>>::value;
      constexpr auto ElemIndex = Size - Index;
      visitor(std::template get<ElemIndex>(tuple));
      NetVariantVisitor<Index - 1>::Process(std::template forward<Visitor>(visitor), std::template forward<Tuple>(tuple));
    }
  };

  template <>
  struct NetVariantVisitor<0>
  {
    template <typename Visitor, typename Tuple>
    static void Process(Visitor && visitor, Tuple && tuple)
    {

    }
  };

  template <typename Visitor, typename Tuple>
  void VisitTuple(Visitor && visitor, Tuple && tuple)
  {
    constexpr auto Size = std::template tuple_size<std::template decay_t<Tuple>>::value;
    NetVariantVisitor<Size>::Process(std::template forward<Visitor>(visitor), std::template forward<Tuple>(tuple));
  }
}
