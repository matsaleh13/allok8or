/**
 * @file std_allocator_adapter.h
 * @brief Adaptor to expose allok8or::Allocator<T> as std::allocator.
 */
#pragma once

// Project headers
#include "allocator.h"

// Library headers
#include <type_traits>

namespace allok8or {

/**
 * @brief Adapter class to expose allok8or::Allocator as std::allocator.
 *
 * Based on: https://howardhinnant.github.io/allocator_boilerplate.html
 * 
 * NOTE: Backing allocator is *shared* on copy/move, and is assumed to be unique
 * for all istances of StdAllocatorAdapter with a given TBackingAllocator.
 * 
 * @tparam T Type of data to be allocated/deallocated.
 * @tparam TBackingAllocator Type of the backing allocator to be passed to
 * allok8or::Allocator. NOTE: Must have const API.
 */
template <typename T, typename TBackingAllocator>
class StdAllocatorAdapter {
  TBackingAllocator& m_allocator;

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

  /**
   * @brief Ctor that takes a TBackingAllocator argument.
   * 
   * @param allocator Allocator instance to be used as the backing allocator.
   */
  StdAllocatorAdapter(Allocator<TBackingAllocator>& allocator) noexcept
      : m_allocator(static_cast<TBackingAllocator&>(allocator)) {}

  template <typename U>
  StdAllocatorAdapter(const StdAllocatorAdapter<U, TBackingAllocator>& other) noexcept
  : m_allocator(static_cast<TBackingAllocator&>(other.allocator())) {}

  value_type* allocate(std::size_t count) {
    return static_cast<value_type*>(
        m_allocator.allocate(count * sizeof(value_type)));
  }

  void deallocate(value_type* user_data, std::size_t) noexcept {
    m_allocator.deallocate(user_data);
  }

  Allocator<TBackingAllocator>& allocator() { return m_allocator; }
  Allocator<TBackingAllocator>& allocator() const { return m_allocator; }

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

  // See https://foonathan.net/blog/2015/10/05/allocatorawarecontainer-propagation-pitfalls.html
	using propagate_on_container_copy_assignment = std::true_type; // for consistency
	using propagate_on_container_move_assignment = std::true_type; // to avoid the pessimization
	using propagate_on_container_swap = std::true_type; // to avoid the undefined behavior

  // using is_always_equal = std::is_empty<allocator>;
};

template <typename T, typename U, typename TBackingAllocator>
bool operator==(StdAllocatorAdapter<T, TBackingAllocator> const& lhs, StdAllocatorAdapter<U, TBackingAllocator> const& rhs) noexcept {
  return lhs.allocator() == rhs.allocator();
}

template <typename T, typename TBackingAllocator, typename U>
bool operator!=(StdAllocatorAdapter<T, TBackingAllocator> const& lhs, StdAllocatorAdapter<U, TBackingAllocator> const& rhs) noexcept {
  return !(lhs == rhs);
}

} // namespace allok8or
