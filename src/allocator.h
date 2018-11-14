/**
 * @file allocator.h
 * @brief CRTP-based Allocator interface class
 * (https://en.wikipedia.org/wiki/Curiously_recurring_template_pattern)
 *
 */
#pragma once

#include <cstddef>

namespace allok8or {

/**
 * @brief Non-abstract interface class for allocators.
 * 
 * @tparam TImplementation Implementation class
 */
template <typename TImplementation>
class Allocator {
public:
  constexpr void* allocate(size_t size) {
    return impl().allocate(size);
  }

  constexpr void* allocate(size_t size, size_t alignment) {
    return impl().allocate(size, alignment);
  }

  constexpr void deallocate(void* data) {
    impl().deallocate(data);
  }

protected:
  Allocator() {}  // Don't create the base class.
  ~Allocator() {} // Don't destroy the base class.

private:
  constexpr TImplementation& impl() {
    return *static_cast<TImplementation*>(this);
  }

};

} // namespace allok8or