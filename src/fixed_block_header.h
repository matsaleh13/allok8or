/**
 * @file fixed_block_header.h
 * @brief Memory block header for managing a single fixed-size allocation.
 */

#pragma once

// Project headers
#include "align.h"
#include "types.h"

// Library headers
#include <cassert>
#include <cstddef>

namespace allok8or {

/**
 * @brief Header for a single block of allocated memory.
 */

/**
 * @brief Header for managing a single fixed-size allocation block within a
 * pool. NOTE: All instances of a FixedBlockHeader within a given pool are of
 * the same size, hence "fixed".
 *
 * @tparam TSize Size of the user data portion of the block (i.e. available to
 * calling code).
 * @tparam TAlign Alignment of the user data portion of the block.
 */
template <size_t TSize, size_t TAlign>
class FixedBlockHeader {
  // Private; to be used only by the static factory method.
  constexpr FixedBlockHeader();

public:
  const static size_t user_data_size;
  const static size_t user_data_alignment;

  constexpr static FixedBlockHeader* create(void* block_start);
  ~FixedBlockHeader() = default;

  FixedBlockHeader(const FixedBlockHeader&) = delete;
  FixedBlockHeader(const FixedBlockHeader&&) = delete;

  // Accessors
  constexpr FixedBlockHeader* next() const { return m_next; }
  void next(FixedBlockHeader* val) { m_next = val; }

  constexpr FixedBlockHeader* prev() const { return m_prev; }
  void prev(FixedBlockHeader* val) { m_prev = val; }

  constexpr void* user_data() const { return m_user_data; }
  constexpr static FixedBlockHeader* get_header(const void* user_data);

private:
  FixedBlockHeader* m_next;
  FixedBlockHeader* m_prev;

  void* m_user_data;
};

template <size_t TSize, size_t TAlign>
const size_t FixedBlockHeader<TSize, TAlign>::user_data_size = TSize;
template <size_t TSize, size_t TAlign>
const size_t FixedBlockHeader<TSize, TAlign>::user_data_alignment = TAlign;

/**
 * @brief Ctor for internal use only.
 */
template <size_t TSize, size_t TAlign>
inline constexpr FixedBlockHeader<TSize, TAlign>::FixedBlockHeader()
    : m_next(nullptr),
      m_prev(nullptr),
      m_user_data(reinterpret_cast<void*>(
          reinterpret_cast<uintptr_t>(this) +
          align::get_aligned_size(sizeof(FixedBlockHeader<TSize, TAlign>),
                                  alignof(FixedBlockHeader<TSize, TAlign>)))) {}

template <size_t TSize, size_t TAlign>
inline constexpr FixedBlockHeader<TSize, TAlign>*
FixedBlockHeader<TSize, TAlign>::create(void* block_start) {
  assert(block_start);
  if (!block_start) {
    return nullptr;
  }

  // Use placement new to init the header in the given memory block.
  FixedBlockHeader<TSize, TAlign>* header =
      new (block_start) FixedBlockHeader<TSize, TAlign>();

  return header;
}

/**
 * @brief Returns the header when given a pointer to user memory.
 *
 * @param Pointer to user memory.
 * @returns Pointer to the FixedBlockHeader for the given user memory.
 */
template <size_t TSize, size_t TAlign>
inline constexpr FixedBlockHeader<TSize, TAlign>*
FixedBlockHeader<TSize, TAlign>::get_header(const void* user_data) {
  return reinterpret_cast<FixedBlockHeader<TSize, TAlign>*>(
      reinterpret_cast<uintptr_t>(user_data) -
      align::get_aligned_size(sizeof(FixedBlockHeader<TSize, TAlign>),
                              alignof(FixedBlockHeader<TSize, TAlign>)));
}

} // namespace allok8or
