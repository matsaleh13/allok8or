/**
 * @file diagnostic_tracking_pool.h
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
 * TODO: RENAME TO DiagnosticBlockPool (or similar)
 *
 * NOTE: Blocks are added only to the head of the doubly-linked list.
 */
class AllocationTrackingPool {
public:
  AllocationTrackingPool();
  ~AllocationTrackingPool();

  // No copies; share when appropriate.
  AllocationTrackingPool(const AllocationTrackingPool&) = delete;
  AllocationTrackingPool& operator=(const AllocationTrackingPool&) = delete;

  bool add(BlockHeader* block);
  bool remove(BlockHeader* block);
  bool in_list(BlockHeader* block) const;
  const BlockHeader* head() const { return m_head; }
  const BlockHeader* tail() const { return m_tail; }

  llong_t num_blocks() const { return m_num_blocks; }
  llong_t num_bytes() const { return m_num_bytes; }

  const AllocationStatsTracker& tracker() const { return *m_stats; }

private:
  BlockHeader* m_head;
  BlockHeader* m_tail;
  llong_t m_num_blocks;
  llong_t m_num_bytes;

  StatsPtr m_stats;
};

} // namespace diagnostic
} // namespace allok8or