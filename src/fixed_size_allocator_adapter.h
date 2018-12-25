/**
 * @file fixed_size_allocator_adapter.h
 * @brief An adapter class for calling an `Allocator` implementation through a
 * `FixedSizeAllocator` interface.
 *
 */
#pragma once

// Project headers
#include "fixed_size_allocator.h"

// Library headers
#include <cstddef>

namespace allok8or {

/**
 * @brief Converts calls to a FixedSizeAllocator API into calls to an Allocator.
 *
 * @tparam TTargetAllocator The target `Allocator` class to receive the calls.
 */
template <typename TTargetAllocator,
          size_t TSize,
          size_t TAlign = alignof(std::max_align_t)>
struct FixedSizeAllocatorAdapter
    : public FixedSizeAllocator<
          FixedSizeAllocatorAdapter<TTargetAllocator, TSize, TAlign>> {

  constexpr explicit FixedSizeAllocatorAdapter(
      const TTargetAllocator& allocator)
      : m_allocator(allocator) {}

  constexpr void* allocate() const {
    return m_allocator.allocate(base::block_size, base::alignment);
  }
  constexpr void deallocate(void* data) const { m_allocator.deallocate(data); }

private:
  using base = FixedSizeAllocatorAdapter<TTargetAllocator, TSize, TAlign>;

  const TTargetAllocator& m_allocator;
};

} // namespace allok8or