#pragma once

#include "NetReflection.h"

namespace NetBindingHelpers
{
  template <class M1, class M2>
  struct NetNoBindingCopy
  {
    void operator ()(const M1 & m1, M2 & m2)
    {
    }
  };

  template <class M1, class M2, class Enable = void>
  struct NetDefaultBindingCopy : public NetNoBindingCopy<M1, M2>
  {

  };

  template <class M1, class M2>
  struct NetDefaultBindingCopy<M1, M2, std::enable_if_t<std::is_convertible<M1, M2>::value>>
  {
    void operator ()(const M1 & m1, M2 & m2) 
    { 
      m2 = static_cast<M2>(m1);
    }
  };

  template <class M1, std::size_t size1, class M2, std::size_t size2>
  struct NetDefaultBindingCopy<M1[size1], M2[size2], typename std::enable_if<std::is_convertible<M1, M2>::value>::type>
  {
    auto operator ()(const M1 * m1, M2 * m2) 
    {
      auto min_size = std::min(size1, size2);

      for (std::size_t index = 0; index < min_size; index++)
      {
        m2[index] = static_cast<M2>(m1[index]);
      }
    }
  };

  template <class UpdateFunc>
  struct NetUpdateOnlyBinding
  {
    UpdateFunc Update;
  };

  template <class Update, class AddFunc, class GetFunc, class RemoveFunc, class ChildBindings>
  struct NetListBindings
  {
    Update Update;
    AddFunc AddElement;
    GetFunc GetElement;
    RemoveFunc RemoveElement;
    ChildBindings GetChildBindings;
  };

  template <class Type, class BoundType, class Bindings, class Enable = void>
  struct NetBindingCopier
  {
    bool operator()(const Type & src, Type & dst, BoundType & bound, Bindings & bindings)
    {
      if (StormReflElementwiseCompare(src, dst) == false)
      {
        StormReflElementwiseCopy(dst, src);
        bindings.Update(dst, bound);
        return true;
      }

      return false;
    }
  };

  template <class Type, class BoundType, class Bindings>
  struct NetBindingCopier<Type, BoundType, Bindings, typename std::enable_if<StormReflCheckReflectable<Type>::value>::type>
  {
    bool operator()(const Type & src, Type & dst, BoundType & bound, Bindings & bindings)
    {
      return bindings(src, dst, bound);
    }
  };

  template <class Type, class BoundType, class Bindings>
  bool NetBindingVectorCopier(const Type & src, Type & dst, BoundType & bound, Bindings & bindings)
  {
    bool changed = false;

    auto child_bindings = bindings.GetChildBindings(dst, bound);

    auto min_size = std::min(src.size(), dst.size());
    NetBindingCopier<typename Type::value_type, typename BoundType::value_type, decltype(child_bindings)> copier;

    for (std::size_t index = 0; index < min_size; index++)
    {
      changed |= copier(src[index], dst[index], bindings.GetElement(dst, bound, index), child_bindings);
    }

    for (std::size_t index = min_size; index < src.size(); index++)
    {
      dst.PushBack(src[index]);
      bindings.AddElement(dst, bound, index);
      changed = true;
    }

    while (dst.size() != src.size())
    {
      auto remove_index = dst.size() - 1;
      bindings.RemoveElement(dst, bound, remove_index);
      dst.RemoveAt(remove_index);
      changed = true;
    }

    if (changed)
    {
      bindings.Update(dst, bound);
    }

    return changed;
  }

  template <class Type, std::size_t size, class BoundType, class Bindings>
  struct NetBindingCopier<NetArrayList<Type, size>, BoundType, Bindings>
  {
    bool operator()(const NetArrayList<Type, size> & src, NetArrayList<Type, size> & dst, BoundType & bound, Bindings & bindings)
    {
      return NetBindingVectorCopier(src, dst, bound, bindings);
    }
  };

  template <class C, class M>
  auto NetMemberPointerInfoDecompose(M C::*val)
  {
    struct Info
    {
      using MemberType = M;
      using ClassType = C;
    };

    return Info{};
  }

  template <bool Match>
  struct NetMatchMemberUpdate
  {
    template <class Type, class Bound>
    static bool Update(const Type & src, Type & dst, Bound & bound)
    {
      return false;
    }

    template <class Type, class Bound, class Bindings>
    static bool Update(const Type & src, Type & dst, Bound & bound, Bindings & bindings)
    {
      return false;
    }
  };

  template <>
  struct NetMatchMemberUpdate<true>
  {
    template <class Type, class Bound>
    static bool Update(const Type & src, Type & dst, Bound & bound)
    {
      auto bindings = NetCreateDefaultBinding<Type, Bound>();
      return NetBindingCopy(src, dst, bound, bindings);
    }

    template <class Type, class Bound, class Bindings>
    static bool Update(const Type & src, Type & dst, Bound & bound, Bindings & bindings)
    {
      return NetBindingCopy(src, dst, bound, bindings);
    }
  };

}
