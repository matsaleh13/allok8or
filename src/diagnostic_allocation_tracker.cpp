/**
 * @file diagnostic_allocation_tracker.cpp
 * @brief Classes used for tracking allocations and deallocations.
 *
 */

// My header
#include "diagnostic_allocation_tracker.h"

// Project headers
#include "align.h"
#include "diagnostic_block_header.h"
#include "logging.h"

// Library headers

namespace allok8or {
namespace diagnostic {

//
// AllocationTracker Implementation
//

/**
 * AllocationTracker ctor
 */
AllocationTracker::AllocationTracker()
    : m_head(nullptr), m_tail(nullptr), m_num_blocks(0), m_num_bytes(0) {}

/**
 * AllocationTracker dtor
 *
 * Logs an error if any are still in use at shutdown.
 */
AllocationTracker::~AllocationTracker() {
  if (m_num_blocks || m_head || m_tail) {
    LOG_ERROR("Detected memory leaks when deleting AllocationTracker "
              "[%d]; leaking [%d] bytes.",
              m_num_blocks,
              m_num_bytes);
  }
}

/**
 * @brief Adds a memory block (with header) to the internal list.
 *
 * @param block Pointer to the block to add.
 * @return true when block added.
 * @return false when failed to add block.
 */
bool AllocationTracker::add(BlockHeader* block) {
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
bool AllocationTracker::remove(BlockHeader* block) {
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

bool AllocationTracker::in_list(BlockHeader* block) const {
  return (m_head == block || m_tail == block || block->next() || block->prev());
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
