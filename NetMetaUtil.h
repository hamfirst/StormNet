#pragma once

#include <type_traits>

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
      std::make_index_sequence<std::tuple_size_v<std::decay_t<Tuple>>>{});
  }
}
