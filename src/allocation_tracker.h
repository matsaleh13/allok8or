/**
 * @file allocation_tracker.h
 * @brief Classes used for tracking allocations and deallocations.
 *
 */
#pragma once

// Project headers
#include "align.h"
#include "diagnostic_util.h"
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
  // Private ctor for use only by the factory (below).
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
  constexpr size_t user_data_size() const { return m_user_data_size; }
  size_t user_data_alignment() const { return m_user_data_alignment; }
  constexpr void* user_data() const { return m_user_data; }

  // Utility methods
  template <size_t N>
  constexpr void get_block_info_string(char (&buffer)[N]) const;
  static BlockHeader* get_header(const void* memory);
  template <typename T>
  constexpr static void set_caller_details(const CallerDetails& caller_details,
                                           const T* user_data);
  constexpr bool is_valid() const;

private:
  void
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
 * Constructor
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
 * Format buffer with header contents so we can log it.
 */
template <size_t N>
inline constexpr void
BlockHeader::get_block_info_string(char (&buffer)[N]) const {
  snprintf(buffer,
           N,
           "type [%s] file [%s] line [%d] size [%d]",
           type_name(),
           m_file_name,
           m_line,
           static_cast<int>(m_user_data_size)); // TODO: size_t or not?
}

/**
 * Returns true if the header's signature matches the global signature.
 */
inline constexpr bool BlockHeader::is_valid() const {
  return m_signature == BLOCK_SIGNATURE;
}

/**
 * Returns the header when given a pointer to user memory.
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
 * Records the caller details and data type in the header.
 * @param caller_details A CallerDetails object containing caller details.
 * @param user_data Pointer to new object that already has a header.
 */
template <typename T>
inline constexpr void
BlockHeader::set_caller_details(const CallerDetails& caller_details,
                                const T* user_data) {
  get_header(reinterpret_cast<const void*>(user_data))
      ->set_caller_details(caller_details.file_name(),
                           caller_details.line(),
                           get_type_name<T>());
}

/**
 * Sets the __FILE__, __LINE__, and data type of the allocation into the header.
 * @param file_name __FILE__ at the allocation call site.
 * @param line __LINE__ at the allocation call site.
 * @param type_name Name of the data type allocated.
 */
inline void BlockHeader::set_caller_details(const char* file_name,
                                            int line,
                                            const char* type_name) {
  if (!is_valid())
    return; // Not a BlockHeader.

  assert(!m_file_name);
  assert(!m_line);
  assert(!m_type_name);

  if (m_file_name || m_line || m_type_name)
    return;

  m_file_name = file_name;
  m_line = line;
  m_type_name = type_name;
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

  bool add_block(BlockHeader* pBlock);
  bool remove_block(BlockHeader* pBlock);
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


/**
 * @brief Overload of operator* that merges caller details with the object
 * allocated.
 *
 * @tparam T Data type of the object to be "stamped".
 * @param caller_details A CallerDetails containing the data to be used to
 * "stamp" the object.
 * @param user_data Pointer to the object to be "stamped".
 * @return T* Pointer to the input object.
 */
template <typename T>
inline constexpr T* operator*(const CallerDetails& caller_details,
                              T* user_data) {
  BlockHeader::set_caller_details<T>(caller_details, user_data);
  return user_data;
}

} // namespace diagnostic
} // namespace allok8or