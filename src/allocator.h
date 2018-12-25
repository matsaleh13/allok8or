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
 * @tparam TImpl Implementation class
 */
template <typename TImpl>
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
  constexpr TImpl& impl() {
    return *static_cast<TImpl*>(this);
  }

  constexpr const TImpl& impl() const {
    return *static_cast<const TImpl*>(this);
  }

template <typename F_TImpl>
  friend constexpr bool operator==(const Allocator<F_TImpl>& lhs,
                          const Allocator<F_TImpl>& rhs);

template <typename F_TImpl>
  friend constexpr bool operator!=(const Allocator<F_TImpl>& lhs,
                          const Allocator<F_TImpl>& rhs);
};

template <typename TImpl>
inline
constexpr bool operator==(const Allocator<TImpl>& lhs,
                          const Allocator<TImpl>& rhs) {
  return lhs.impl() == rhs.impl();
}

template <typename TImpl>
inline
constexpr bool operator!=(const Allocator<TImpl>& lhs,
                          const Allocator<TImpl>& rhs) {
  return lhs.impl() != rhs.impl();
}

} // namespace allok8or