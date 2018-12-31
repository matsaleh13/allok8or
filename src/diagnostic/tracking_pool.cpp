/**
 * @file diagnostic_tracking_pool.cpp
 * @brief Classes used for tracking allocations and deallocations.
 *
 */

// My header
#include "tracking_pool.h"

// Project headers
#include "../align.h"
#include "allocation_stats.h"
#include "block_header.h"
#include "allocation_stats_reporter.h"
#include "../logging.h"

// Library headers
#include <memory>

namespace allok8or {
namespace diagnostic {

//
// AllocationTrackingPool Implementation
//

/**
 * AllocationTrackingPool ctor
 */
AllocationTrackingPool::AllocationTrackingPool()
    : m_head(nullptr),
      m_tail(nullptr),
      m_num_blocks(0),
      m_num_bytes(0),
      m_stats(std::make_unique<AllocationStatsTracker>()) {}

/**
 * AllocationTrackingPool dtor
 *
 * Logs an error if any are still in use at shutdown.
 */
AllocationTrackingPool::~AllocationTrackingPool() {
  if (m_num_blocks || m_head || m_tail) {
    LOG_ERROR("Detected memory leaks when deleting AllocationTrackingPool "
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
bool AllocationTrackingPool::add(BlockHeader* block) {
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

  m_stats->track_allocation(block->type_name(),
                    block->file_name(),
                    block->line(),
                    block->user_data_size());
  return true;
}

/**
 * @brief Removes a memory block (with header) from the internal list.
 *
 * @param block Pointer to the block to remove.
 * @return true when block removed.
 * @return false when failed to remove block.
 */
bool AllocationTrackingPool::remove(BlockHeader* block) {
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

  m_stats->track_deallocation(block->type_name(),
                      block->file_name(),
                      block->line(),
                      block->user_data_size());

  return true;
}

bool AllocationTrackingPool::in_list(BlockHeader* block) const {
  return (m_head == block || m_tail == block || block->next() || block->prev());
}


} // namespace diagnostic
} // namespace allok8or
