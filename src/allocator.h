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
  constexpr void* allocate(size_t size) const { return impl().allocate(size); }

  constexpr void* allocate(size_t size, size_t alignment) const {
    return impl().allocate(size, alignment);
  }

  constexpr void deallocate(void* data) const { impl().deallocate(data); }

protected:
  constexpr Allocator() {}  // Don't create the base class.

private:
  constexpr TImplementation& impl() {
    return *static_cast<TImplementation*>(this);
  }

  constexpr const TImplementation& impl() const {
    return *static_cast<const TImplementation*>(this);
  }

template <typename F_TImplementation>
  friend constexpr bool operator==(const Allocator<F_TImplementation>& lhs,
                          const Allocator<F_TImplementation>& rhs);

template <typename F_TImplementation>
  friend constexpr bool operator!=(const Allocator<F_TImplementation>& lhs,
                          const Allocator<F_TImplementation>& rhs);
};

template <typename TImplementation>
inline
constexpr bool operator==(const Allocator<TImplementation>& lhs,
                          const Allocator<TImplementation>& rhs) {
  return lhs.impl() == rhs.impl();
}

template <typename TImplementation>
inline
constexpr bool operator!=(const Allocator<TImplementation>& lhs,
                          const Allocator<TImplementation>& rhs) {
  return lhs.impl() != rhs.impl();
}

} // namespace allok8or