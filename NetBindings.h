#pragma once

#include "NetBindingHelpers.h"

inline auto NetBindingIgnoreUpdate()
{
  return [](auto a, auto b) {};
}

template <class C1, class M1, class C2, class M2>
inline auto NetCreateDefaultMemberBinding(M1 C1::*ptr1, M2 C2::*ptr2)
{
  return NetBindingHelpers::NetUpdateOnlyBinding<NetBindingHelpers::NetDefaultBindingCopy<M1, M2>>{};
}

template <class C1, class C2>
inline auto NetCreateDefaultBinding()
{
  return NetBindingHelpers::NetUpdateOnlyBinding<NetBindingHelpers::NetDefaultBindingCopy<C1, C2>>{};
}

template <class Type, std::size_t size, class Container2, class Update, class ChildBindings>
inline auto NetCreateDefaultArrayListContainerBinding(Update & update, ChildBindings & child_bindings)
{
  auto add_func = [&](const NetArrayList<Type, size> & src, Container2 & container, std::size_t index) 
  { 
    container.emplace(container.begin() + index); child_bindings.Update(src[index], container[index]); 
  };

  auto get_func = [&](const NetArrayList<Type, size> & src, Container2 & container, std::size_t index) -> decltype(auto) { return container[index]; };
  auto rem_func = [&](const NetArrayList<Type, size> & src, Container2 & container, std::size_t index) { container.erase(container.begin() + index); };
  auto get_child_bindngs = [&](const NetArrayList<Type, size> & src, Container2 & container) { return child_bindings; };

  return NetBindingHelpers::NetListBindings<Update &, decltype(add_func), decltype(get_func), decltype(rem_func), decltype(get_child_bindngs)>
  { update, add_func, get_func, rem_func, get_child_bindngs };
}

template <class C1, class M1, std::size_t size, class C2, class Container2, class Update, class ChildBindings>
inline auto NetCreateDefaultContainerBinding(NetArrayList<M1, size> C1::*container1, Container2 C2::*container2, Update & update, ChildBindings & child_bindings)
{
  return NetCreateDefaultArrayListContainerBinding<M1, size, Container2, Update, ChildBindings>(update, child_bindings);
}

template <class C1, class M1, class C2, class M2, M1 C1::*ptr1, M2 C2::*ptr2, class Bindings>
auto NetCreateMemberBinding(Bindings & bindings)
{
  auto member_update = [=](auto & src, auto & dst, C2 & bound, auto field_data, bool & matched)
  {
    constexpr auto member_pointer = field_data.GetMemberPtr();
    auto & bound_member = (&bound)->*ptr2;

    matched |= StormReflCompareMemberPointers(ptr1, member_pointer);
    return NetBindingHelpers::NetMatchMemberUpdate<StormReflCompareMemberPointers(ptr1, member_pointer)>::Update(src, dst, bound_member, bindings);
  };

  return member_update;
}

template <class C1, class M1, class C2, class M2, M1 C1::*ptr1, M2 C2::*ptr2>
auto NetCreateMemberBinding()
{
  auto member_update = [](auto & src, auto & dst, C2 & bound, auto field_data, bool & matched)
  {
    constexpr auto member_pointer = field_data.GetMemberPtr();
    auto & bound_member = (&bound)->*ptr2;

    matched |= StormReflCompareMemberPointers(ptr1, member_pointer);
    return NetBindingHelpers::NetMatchMemberUpdate<StormReflCompareMemberPointers(ptr1, member_pointer)>::Update(src, dst, bound_member);
  };

  return member_update;
}

inline auto NetCreateStructBindingList()
{
  return [](auto & src, auto & dst, auto & bound, auto field_data)
  {
    return StormReflCompareAndCopy(dst, src);
  };
}

template <class FirstMemberBinding, class ... RemainingMemberBindings>
auto NetCreateStructBindingList(FirstMemberBinding && first_binding, RemainingMemberBindings &&... other_bindings)
{
  auto remaining_binding = NetCreateStructBindingList(other_bindings...);

  return [=](auto & src, auto & dst, auto & bound, auto field_data)
  {
    bool matched = false;
    bool changed = first_binding(src, dst, bound, field_data, matched);
    if (matched == false)
    {
      changed |= remaining_binding(src, dst, bound, field_data);
    }
    return changed;
  };
}

template <class Bindings>
auto NetCreateStructBinding(Bindings && bindings)
{
  return [=](auto & src, auto & dst, auto & bound)
  {
    bool changed = false;

    auto elem_visitor = [&](auto & src_field, auto dst_field)
    {
      auto & src_member = src_field.Get();
      auto & dst_member = dst_field.Get();

      changed |= bindings(src_member, dst_member, bound, src_field);
    };

    StormReflVisitEach(src, dst, elem_visitor);
    return changed;
  };
}

#define NET_CREATE_MEMBER_BINDING(member_ptr1, member_ptr2) \
  NetCreateMemberBinding< \
    decltype(NetBindingHelpers::NetMemberPointerInfoDecompose(member_ptr1))::ClassType, \
    decltype(NetBindingHelpers::NetMemberPointerInfoDecompose(member_ptr1))::MemberType, \
    decltype(NetBindingHelpers::NetMemberPointerInfoDecompose(member_ptr2))::ClassType, \
    decltype(NetBindingHelpers::NetMemberPointerInfoDecompose(member_ptr2))::MemberType, \
    member_ptr1, member_ptr2>()

#define NET_CREATE_MEMBER_BINDING_CHILD(member_ptr1, member_ptr2, child_bindings) \
  NetCreateMemberBinding< \
    decltype(NetBindingHelpers::NetMemberPointerInfoDecompose(member_ptr1))::ClassType, \
    decltype(NetBindingHelpers::NetMemberPointerInfoDecompose(member_ptr1))::MemberType, \
    decltype(NetBindingHelpers::NetMemberPointerInfoDecompose(member_ptr2))::ClassType, \
    decltype(NetBindingHelpers::NetMemberPointerInfoDecompose(member_ptr2))::MemberType, \
    member_ptr1, member_ptr2, decltype(child_bindings)>(child_bindings)


#define NET_DECLARE_STRUCT_BINDING(refl_struct, client_struct) \
  class __Binding##refl_struct##client_struct \
  { \
  public: \
    bool operator()(const refl_struct & src, refl_struct & dst, client_struct & bound); \
  }; \

#define NET_DEFINE_STRUCT_BINDING(refl_struct, client_struct, ...) \
  bool __Binding##refl_struct##client_struct::operator()(const refl_struct & src, refl_struct & dst, client_struct & bound) \
  { \
    auto bindings = NetCreateStructBindingList( \
      __VA_ARGS__ \
    ); \
    bool changed = false; \
    auto elem_visitor = [&](auto & src_field, auto dst_field) \
    { \
      auto & src_member = src_field.Get(); \
      auto & dst_member = dst_field.Get(); \
      changed |= bindings(src_member, dst_member, bound, src_field); \
    }; \
    StormReflVisitEach(src, dst, elem_visitor); \
    return changed; \
  } \

#define NET_GET_STRUCT_BINDING(refl_struct, client_struct)  __Binding##refl_struct##client_struct {}

template <class Base, class Bound, class Bindings>
bool NetBindingCopy(const Base & src, Base & dst, Bound & bound, Bindings && bindings)
{
  NetBindingHelpers::NetBindingCopier<Base, Bound, Bindings> copier;
  return copier(src, dst, bound, bindings);
}


