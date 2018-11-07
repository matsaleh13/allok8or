/**
 * @file allocation_tracker.cpp
 * @brief Classes used for tracking allocations and deallocations.
 *
 */

// My header
#include "allocation_tracker.h"

// Project headers
#include "align.h"
#include "logging.h"

// Library headers

namespace allok8or {
namespace diagnostic {

//
// BlockSignature implementation.
//
const BlockSignature BlockHeader::BLOCK_SIGNATURE;

//
// BlockHeader Implementation
//

/**
 * @brief Factory method for ensuring ctor params are valid.
 *
 * @param block_start Start address of the complete memory block (and pointer to
 * the BlockHeader).
 * @param user_data_size Size of the user portion of the memory block.
 * @param user_data_alignment Alignment of the data in the user portion of the
 * memory block.
 * @param type_name String representation of the data type of the user memory.
 * @param file_name File name of the caller details.
 * @param line Line number of the caller details.
 * @return BlockHeader* Pointer to the full memory block.
 */
BlockHeader* BlockHeader::create(void* block_start, size_t user_data_size,
                                 size_t user_data_alignment,
                                 const char* type_name /*= nullptr*/,
                                 const char* file_name /*= nullptr*/,
                                 int line /*= 0 */) {
  assert(block_start);
  assert(user_data_size);
  assert(user_data_alignment);

  if (!block_start || !user_data_size || !user_data_alignment)
    return nullptr;

  BlockHeader* header = new (block_start) BlockHeader(
      user_data_size, user_data_alignment, file_name, line, type_name);

  return header;
}

//
// AllocationTracker Implementation
//

/**
 * AllocationTracker ctor
 */
AllocationTracker::AllocationTracker()
    : m_head(nullptr), m_tail(nullptr), m_num_blocks(0) {}

/**
 * AllocationTracker dtor
 *
 * Logs an error if any are still in use at shutdown.
 */
AllocationTracker::~AllocationTracker() {
  if (m_num_blocks || m_head || m_tail) {
    LOG_ERROR(L"Detected memory leaks when deleting AllocationTracker "
              L"[%d]; leaking [%d] bytes.",
              m_num_blocks, m_num_bytes);
  }
}

/**
 * @brief Adds a memory block (with header) to the internal list.
 * 
 * @param block Pointer to the block to add.
 * @return true when block added.
 * @return false when failed to add block.
 */
bool AllocationTracker::add_block(BlockHeader* block) {
  assert(block);
  assert(!in_list(block));

  if (!block)
    return false;
  if (in_list(block))
    return false;

  // We only add to the head...
  if (m_head) {
    block->next(m_head);
    m_head->prev(block);
  }
  m_head = block;

  // ... except when we have an empty list.
  if (!m_tail) {
    m_tail = block;
  }

  m_num_blocks++;
  m_num_bytes += block->user_data_size();

  return true;
}

/**
 * @brief Removes a memory block (with header) from the internal list.
 * 
 * @param block Pointer to the block to remove.
 * @return true when block removed.
 * @return false when failed to remove block.
 */
bool AllocationTracker::remove_block(BlockHeader* block) {
  assert(block);
  assert(in_list(block));
  assert(m_head);

  if (!block)
    return false;
  if (!in_list(block))
    return false;
  if (!m_head)
    return false;

  if (block->prev()) {
    auto prev = block->prev();
    assert(prev->next() == block);

    prev->next(block->next());
  }

  if (block->next()) {
    auto next = block->next();
    assert(next->prev() == block);

    next->prev(block->prev());
  }

  if (block == m_head) {
    m_head = block->next();
  }

  if (block == m_tail) {
    m_tail = block->prev();
  }

  block->prev(nullptr);
  block->next(nullptr);

  m_num_blocks--;
  m_num_bytes -= block->user_data_size();

  if (m_num_blocks == 0) {
    m_head = nullptr;
    m_tail = nullptr;
  }

  return true;
}

/**
 * @brief Logs blocks with no formatting or summary.
 * 
 */
void AllocationTracker::log_raw_blocks() {
  auto block = m_head;
  while (block) {
    char buffer[256];
    block->get_block_info_string(buffer);
    LOG_INFO(buffer);

    block = block->next();
  }
}

/**
 * @brief Summarizes net allocations by type and/or call site.
 * 
 */
void AllocationTracker::log_block_summary() {
  // TODO
}

} // namespace diagnostic
} // namespace allok8or
