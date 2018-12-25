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
template <typename TImpl,
          size_t TSize,
          size_t TAlign = alignof(std::max_align_t)>
class FixedSizeAllocator {
public:
  using const static size_t block_size = TSize;
  using const static size_t alignment = TAlign;

  constexpr void* allocate() const { return impl().allocate(); }
  constexpr void deallocate(void* data) const { impl().deallocate(data); }

protected:
  constexpr FixedSizeAllocator() {} // Don't create the base class.

private:
  constexpr TImpl& impl() { return *static_cast<TImpl*>(this); }

  constexpr const TImpl& impl() const {
    return *static_cast<const TImpl*>(this);
  }

  template <typename F_TImpl, size_t F_TSize, size_t F_TAlign>
  friend constexpr bool
  operator==(const FixedSizeAllocator<F_TImpl, F_TSize, F_TAlign>& lhs,
             const FixedSizeAllocator<F_TImpl, F_TSize, F_TAlign>& rhs);

  template <typename F_TImpl, size_t F_TSize, size_t F_TAlign>
  friend constexpr bool
  operator!=(const FixedSizeAllocator<F_TImpl, F_TSize, F_TAlign>& lhs,
             const FixedSizeAllocator<F_TImpl, F_TSize, F_TAlign>& rhs);
};

template <typename F_TImpl, size_t F_TSize, size_t F_TAlign>
inline constexpr bool
operator==(const FixedSizeAllocator<F_TImpl, F_TSize, F_TAlign>& lhs,
           const FixedSizeAllocator<F_TImpl, F_TSize, F_TAlign>& rhs) {
  return lhs.impl() == rhs.impl();
}

template <typename F_TImpl, size_t F_TSize, size_t F_TAlign>
inline constexpr bool
operator!=(const FixedSizeAllocator<F_TImpl, F_TSize, F_TAlign>& lhs,
           const FixedSizeAllocator<F_TImpl, F_TSize, F_TAlign>& rhs) {
  return lhs.impl() != rhs.impl();
}

} // namespace allok8or