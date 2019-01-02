/**
 * @file allocator_fixed.h
 * @brief CRTP-based FixedSizeAllocator interface class
 * (https://en.wikipedia.org/wiki/Curiously_recurring_template_pattern)
 *
 */
#pragma once

#include <cstddef>

namespace allok8or {

/**
 * @brief Non-abstract interface class for fixed-size allocators.
 * @note Not a specializtion of or derived from `Allocator`.
 *
 * @tparam TImpl Implementation class.
 * @tparam TSize Size of the blocks this allocator creates.
 * @tparam TAlign Memory alignment of the blocks this allocator creates.
 */
template <typename TImpl>
class FixedSizeAllocator {
public:
  const static size_t block_size = TImpl::TSize;
  const static size_t alignment = TImpl::TAlign;

  constexpr void* allocate() const { return impl().allocate(); }
  constexpr void deallocate(void* data) const { impl().deallocate(data); }

protected:
  constexpr FixedSizeAllocator() {} // Don't create the base class.

private:
  constexpr TImpl& impl() { return *static_cast<TImpl*>(this); }

  constexpr const TImpl& impl() const {
    return *static_cast<const TImpl*>(this);
  }

  template <typename F_TImpl>
  friend constexpr bool
  operator==(const FixedSizeAllocator<F_TImpl>& lhs,
             const FixedSizeAllocator<F_TImpl>& rhs);

  template <typename F_TImpl>
  friend constexpr bool
  operator!=(const FixedSizeAllocator<F_TImpl>& lhs,
             const FixedSizeAllocator<F_TImpl>& rhs);
};

template <typename F_TImpl>
inline constexpr bool
operator==(const FixedSizeAllocator<F_TImpl>& lhs,
           const FixedSizeAllocator<F_TImpl>& rhs) {
  return lhs.impl() == rhs.impl();
}

template <typename F_TImpl>
inline constexpr bool
operator!=(const FixedSizeAllocator<F_TImpl>& lhs,
           const FixedSizeAllocator<F_TImpl>& rhs) {
  return lhs.impl() != rhs.impl();
}

} // namespace allok8or