/**
 * @file diagnostic_allocation_tracker.h
 * @brief Classes used for tracking allocations and deallocations.
 *
 */
#pragma once

// Project headers
#include "types.h"

// Library headers
#include <memory>

namespace allok8or {
namespace diagnostic {

class BlockHeader;
class AllocationStatsTracker;

typedef std::unique_ptr<AllocationStatsTracker> StatsPtr;

/**
 * Manages the linked list of headers and generates metrics from them.
 *
 * NOTE: Blocks are added only to the head of the doubly-linked list.
 */
class AllocationTracker {
public:
  AllocationTracker();
  ~AllocationTracker();

  // No copies; share when appropriate.
  AllocationTracker(const AllocationTracker&) = delete;
  AllocationTracker& operator=(const AllocationTracker&) = delete;

  bool add(BlockHeader* block);
  bool remove(BlockHeader* block);
  bool in_list(BlockHeader* block) const;
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

  StatsPtr m_stats;
};

} // namespace diagnostic
} // namespace allok8or