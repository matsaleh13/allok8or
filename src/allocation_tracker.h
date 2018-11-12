/**
 * @file allocation_tracker.h
 * @brief Classes used for tracking allocations and deallocations.
 *
 */
#pragma once

// Project headers
#include "align.h"
#include "diagnostic_util.h"
#include "type_name_helper.h"
#include "types.h"

// Library headers
#include <cassert>
#include <cstddef>
#include <cstdio>

namespace allok8or {
namespace diagnostic {

/**
 * @brief Identifies a block of memory that can be tracked.
 */
struct BlockSignature {
  constexpr BlockSignature()
      : m_sig0('ALK8'), m_sig1('BLCK'), m_sig2('HEDR'), m_sig3('ALK8') {}

  constexpr bool operator==(const BlockSignature& rhs) const {
    return m_sig0 == rhs.m_sig0 && m_sig1 == rhs.m_sig1 &&
           m_sig2 == rhs.m_sig2 && m_sig3 == rhs.m_sig3;
  }

  uint_t m_sig0;
  uint_t m_sig1;
  uint_t m_sig2;
  uint_t m_sig3;
};

/**
 * @brief Header for a single block of allocated memory.
 *
 * Contains diagnostic data used for keeping track of a block of memory.
 */
class BlockHeader {
  // Private; to be used only by the static factory method.
  BlockHeader(size_t user_data_size,
              size_t user_data_alignment,
              const char* file_name = nullptr,
              int line = 0,
              const char* type_name = nullptr);

public:
  static BlockHeader* create(void* block_start,
                             size_t user_data_size,
                             size_t user_data_alignment,
                             const char* type_name = nullptr,
                             const char* file_name = nullptr,
                             int line = 0);
  ~BlockHeader() = default;

  BlockHeader() = delete;
  BlockHeader(const BlockHeader&) = delete;
  BlockHeader(const BlockHeader&&) = delete;

  // Accessors
  constexpr BlockHeader* next() const { return m_next; }
  void next(BlockHeader* val) { m_next = val; }

  constexpr BlockHeader* prev() const { return m_prev; }
  void prev(BlockHeader* val) { m_prev = val; }

  constexpr const char* type_name() const { return m_type_name; }
  constexpr const char* file_name() const { return m_file_name; }
  constexpr int line() const { return m_line; }

  constexpr size_t user_data_size() const { return m_user_data_size; }
  size_t user_data_alignment() const { return m_user_data_alignment; }

  constexpr void* user_data() const { return m_user_data; }

  // Utility methods
  template <size_t N>
  constexpr void get_block_info_string(char (&buffer)[N]) const;
  static BlockHeader* get_header(const void* memory);
  template <typename T>
  constexpr static bool set_caller_details(const CallerDetails& caller_details,
                                           const T* user_data);
  constexpr bool is_valid() const;

private:
  constexpr bool
  set_caller_details(const char* file_name, int line, const char* type_name);

  BlockHeader* m_next;
  BlockHeader* m_prev;

  size_t m_user_data_size;
  size_t m_user_data_alignment;

  mutable const char* m_file_name;
  mutable int m_line;
  mutable const char* m_type_name;

  static const BlockSignature BLOCK_SIGNATURE;

  const BlockSignature m_signature;

  void* m_user_data;

  // TODO: timestamp
};

/**
 * @brief Ctor for internal use only.
 * 
 * NOTE: file_name, line, and type_name are reserved for special cases, 
 *       not yet implemented. Those cases will probably require an overload of
 *       the factory method. TODO: remove these until needed?
 * 
 * @param user_data_size Size of memory block requested by the caller.
 * @param user_data_alignment Alignment requested by the caller.
 * @param file_name __FILE__ at the call site, or null (default).
 * @param line __LINE__ at the call site, or 0 (default).
 * @param type_name String representation of the type for which the block is created, or null (default).
 */
inline BlockHeader::BlockHeader(size_t user_data_size,
                                size_t user_data_alignment,
                                const char* file_name /*= nullptr*/,
                                int line /*= 0*/,
                                const char* type_name /*= nullptr */)
    : m_next(nullptr),
      m_prev(nullptr),
      m_user_data_size(user_data_size),
      m_user_data_alignment(user_data_alignment),
      m_file_name(file_name),
      m_line(line),
      m_type_name(type_name),
      m_user_data(reinterpret_cast<void*>(
          reinterpret_cast<uintptr_t>(this) +
          align::get_aligned_size(sizeof(BlockHeader), alignof(BlockHeader)))) {
}

/**
 * @brief Format buffer with header contents so we can log it.
 * 
 * @tparam N 
 * @param buffer Character buffer to fill with the formatted data.
 * 
 */
template <size_t N>
inline constexpr void
BlockHeader::get_block_info_string(char (&buffer)[N]) const {
  snprintf(buffer,
           N,
           "type [%s] file [%s] line [%d] size [%d]",
           type_name(),
           file_name(),
           line(),
           static_cast<int>(m_user_data_size)); // TODO: size_t or not?
}

/**
 * @brief Verifies that this header is representing a block of memory created for the header.
 * 
 * @return true When the header has a signature that matches the global one.
 * @return false When the header does not have a signature that matches the global one.
 */
inline constexpr bool BlockHeader::is_valid() const {
  return m_signature == BLOCK_SIGNATURE;
}

/**
 * @brief Returns the header when given a pointer to user memory.
 *
 * @param Pointer to user memory.
 * @returns Pointer to the BlockHeader for the given user memory.
 */
inline BlockHeader* BlockHeader::get_header(const void* user_data) {
  return reinterpret_cast<BlockHeader*>(
      reinterpret_cast<uintptr_t>(user_data) -
      align::get_aligned_size(sizeof(BlockHeader), alignof(BlockHeader)));
}

/**
 * @brief Records the caller details and data type in the header.
 * 
 * @param caller_details A CallerDetails object containing caller details.
 * @param user_data Pointer to new object that already has a header.
 * @return true When the header is valid and call details haven't been set yet.
 * @return false When the header is NOT valid OR call details HAVE alread been set.
 */
template <typename T>
inline constexpr bool
BlockHeader::set_caller_details(const CallerDetails& caller_details,
                                const T* user_data) {
  return get_header(reinterpret_cast<const void*>(user_data))
      ->set_caller_details(caller_details.file_name(),
                           caller_details.line(),
                           get_type_name<T>());
}

/**
 * @brief Sets the __FILE__, __LINE__, and data type of the allocation into the header.
 * 
 * @param file_name __FILE__ at the allocation call site.
 * @param line __LINE__ at the allocation call site.
 * @param type_name Name of the data type allocated.
 * @return true When the header is valid and call details haven't been set yet.
 * @return false When the header is NOT valid OR call details HAVE alread been set.
 */
inline constexpr bool BlockHeader::set_caller_details(const char* file_name,
                                                      int line,
                                                      const char* type_name) {
  if (!is_valid())
    return false; // Not a BlockHeader.

  if (m_file_name || m_line || m_type_name)
    return false;  // Already set

  m_file_name = file_name;
  m_line = line;
  m_type_name = type_name;

  return true;
}

/**
 * Manages the linked list of headers and generates metrics from them.
 *
 * NOTE: Blocks are added only to the head of the doubly-linked list.
 */
class AllocationTracker {
public:
  AllocationTracker();
  ~AllocationTracker();

  bool add(BlockHeader* pBlock);
  bool remove(BlockHeader* pBlock);
  bool in_list(BlockHeader* pBlock) const {
    return (m_head == pBlock || m_tail == pBlock || pBlock->next() ||
            pBlock->prev());
  }
  const BlockHeader* head() const { return m_head; }
  const BlockHeader* tail() const { return m_tail; }

  llong_t num_blocks() const { return m_num_blocks; }
  llong_t num_bytes() const { return m_num_bytes; }

  void log_raw_blocks();
  void log_block_summary();

private:
  BlockHeader* m_head;
  BlockHeader* m_tail;
  llong_t m_num_blocks;
  llong_t m_num_bytes;
};

} // namespace diagnostic
} // namespace allok8or