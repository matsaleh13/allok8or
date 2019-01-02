/**
 * @file block_allocator.h
 * @brief Responsible for allocating and deallocating blocks of a fixed size and
 * caching them in a pool.
 *
 */
#pragma once

// Project headers
#include "fixed_block_header.h"
#include "fixed_block_pool.h"
#include "fixed_size_allocator.h"

// Library headers
#include <cstddef>

namespace allok8or {
template <size_t TSize, size_t TAlign, typename TPageAllocator>
class BlockAllocator
    : public FixedSizeAllocator<BlockAllocator<TSize, TAlign>> {
public:
  constexpr BlockAllocator() = default;

  // No copies; share this when appropriate.
  BlockAllocator(const BlockAllocator&) = delete;
  BlockAllocator& operator=(const BlockAllocator&) = delete;
  BlockAllocator(const BlockAllocator&&) = delete;
  BlockAllocator& operator=(const BlockAllocator&&) = delete;

  // Allocation API
  constexpr void* allocate() const;
  constexpr void deallocate(void* user_data) const;

private:
  PageHeader* get_page();

  using BlockHeaderT = FixedBlockHeader<TSize, TAlign>;
  using PoolT = FixedBlockPool<BlockHeaderT>;

  PoolT m_pool; 
  TPageAllocator m_page_allocator;
};

template <size_t TSize, size_t TAlign, typename TPageAllocator>
inline constexpr bool
operator==(const BlockAllocator<TSize, TAlign, TPageAllocator>& lhs,
           const BlockAllocator<TSize, TAlign, TPageAllocator>& rhs) {
  return &lhs == &rhs;
}

template <size_t TSize, size_t TAlign, typename TPageAllocator>
inline constexpr bool
operator!=(const BlockAllocator<TSize, TAlign, TPageAllocator>& lhs,
           const BlockAllocator<TSize, TAlign, TPageAllocator>& rhs) {
  return !(&lhs == &rhs);
}

template <size_t TSize, size_t TAlign, typename TPageAllocator>
inline constexpr void*
BlockAllocator<TSize, TAlign, TPageAllocator>::allocate() const {
  auto block = m_pool.get();
  if (!block) {
    auto page = m_page_allocator.allocate();
  }

  m_used_pool.add(block);
}

} // namespace allok8or
