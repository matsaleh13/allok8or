/**
 * @file std_allocator_adapter.h
 * @brief Adaptor to expose allok8or::Allocator<T> as std::allocator.
 */
#pragma once

#include "allocator.h"

namespace allok8or {

/**
 * @brief Adapter class to expose allok8or::Allocator as std::allocator.
 *
 * Based on: https://howardhinnant.github.io/allocator_boilerplate.html
 * 
 * @tparam T Type of data to be allocated/deallocated.
 * @tparam TAllocator Type of the backing allocator to be passed to
 * allok8or::Allocator. NOTE: Must have const API.
 */
template <typename T, typename TAllocator>
class StdAllocatorAdapter {
  const Allocator<TAllocator>& m_allocator;

public:
  using value_type = T;

  // using pointer = value_type*;
  // using const_pointer =
  //     typename std::pointer_traits<pointer>::template rebind<value_type
  //     const>;
  // using void_pointer =
  //     typename std::pointer_traits<pointer>::template rebind<void>;
  // using const_void_pointer =
  //     typename std::pointer_traits<pointer>::template rebind<const void>;

  // using difference_type =
  //     typename std::pointer_traits<pointer>::difference_type;
  // using size_type = std::make_unsigned_t<difference_type>;

  // template <class U>
  // struct rebind {
  //   typedef allocator<U> other;
  // };

  StdAllocatorAdapter() = delete;

  StdAllocatorAdapter(const Allocator<TAllocator>& allocator) noexcept
      : m_allocator(allocator) {}

  template <typename U>
  StdAllocatorAdapter(const StdAllocatorAdapter<U, TAllocator>& other) noexcept
  : m_allocator(other.allocator()) {}

  value_type* allocate(std::size_t count) {
    return static_cast<value_type*>(
        m_allocator.allocate(count * sizeof(value_type)));
  }

  void deallocate(value_type* user_data, std::size_t) noexcept {
    m_allocator.deallocate(user_data);
  }

  Allocator<TAllocator>& allocator() { return m_allocator; }

  const Allocator<TAllocator>& allocator() const { return m_allocator; }

  // value_type* allocate(std::size_t n, const_void_pointer) {
  //   return allocate(n);
  // }

  // template <class U, class... Args>
  // void construct(U* p, Args&&... args) {
  //   ::new (p) U(std::forward<Args>(args)...);
  // }

  // template <class U>
  // void destroy(U* p) noexcept {
  //   p->~U();
  // }

  // std::size_t max_size() const noexcept {
  //   return std::numeric_limits<size_type>::max();
  // }

  // allocator select_on_container_copy_construction() const { return *this; }

  // using propagate_on_container_copy_assignment = std::false_type;
  // using propagate_on_container_move_assignment = std::false_type;
  // using propagate_on_container_swap = std::false_type;
  // using is_always_equal = std::is_empty<allocator>;
};

template <typename T, typename U, typename TAllocator>
bool operator==(StdAllocatorAdapter<T, TAllocator> const& lhs, StdAllocatorAdapter<U, TAllocator> const& rhs) noexcept {
  return lhs.allocator() == rhs.allocator();
}

template <typename T, typename TAllocator, typename U>
bool operator!=(StdAllocatorAdapter<T, TAllocator> const& lhs, StdAllocatorAdapter<U, TAllocator> const& rhs) noexcept {
  return !(lhs == rhs);
}

} // namespace allok8or
