/**
 * @file diagnostic_allocation_stats.h
 * @brief Class for keeping track of allocation statistics.
 *
 */
#pragma once

// Project headers
#include "pass_through.h"
#include "std_allocator_adapter.h"
#include "types.h"

// Library headers
#include <functional>
#include <unordered_map>

namespace allok8or {
namespace diagnostic {

struct AllocationStatsKey {
  const char* type_name;
  const char* file_name;
  int line;

  struct hash {
    size_t operator()(const AllocationStatsKey& k) const {
      auto h1 = std::hash<const char*>()(k.type_name);
      auto h2 = std::hash<const char*>()(k.file_name);
      auto h3 = std::hash<int>()(k.line);

      return h1 ^ (h2 << 1) ^ (h3 << 1);
    }
  };

  struct equal_to {
    constexpr size_t operator()(const AllocationStatsKey& lhs,
                                const AllocationStatsKey& rhs) const {
      return (&lhs == &rhs) ||
             (lhs.type_name == rhs.type_name &&
              lhs.file_name == rhs.file_name && lhs.line == rhs.line);
    }
  };
};

struct AllocationStats {
  static const AllocationStats null_stats;

  llong_t allocations = 0;
  llong_t bytes_allocated = 0;

  llong_t deallocations = 0;
  llong_t bytes_deallocated = 0;

  llong_t net_allocations() const { return allocations - deallocations; };
  llong_t net_bytes() const { return bytes_allocated - bytes_deallocated; };
};

// Static init.
const AllocationStats AllocationStats::null_stats;

/**
 * @brief Responsible for recording and reporting cumulative statistics on
 * allocations and deallocations.
 *
 */
class AllocationStatsTracker {
public:
  //
  // Types
  //

  using value_type = std::pair<const AllocationStatsKey, AllocationStats>;
  // PassThroughAllocator calls system allocation APIs directly; no new/delete.
  using allocator_type = StdAllocatorAdapter<value_type, PassThroughAllocator>;
  typedef std::unordered_map<AllocationStatsKey,
                             AllocationStats,
                             AllocationStatsKey::hash,
                             AllocationStatsKey::equal_to,
                             allocator_type>
      StatsMap;

  //
  // dtor/dtor
  //
  AllocationStatsTracker() : m_stats(m_allocator) {}

  //
  // API
  //

  void allocate(const char* type_name,
                const char* file_name,
                int line,
                size_t bytes);

  void allocate(const AllocationStatsKey& key, size_t bytes);

  void deallocate(const char* type_name,
                  const char* file_name,
                  int line,
                  size_t bytes);

  void deallocate(const AllocationStatsKey& key, size_t bytes);

  const StatsMap& stats() const { return m_stats; }
  const AllocationStats& stats(const AllocationStatsKey& key) const;

private:
  static allocator_type::backing_allocator_type
      m_allocator; // stateless, so static is safe.
  StatsMap m_stats;
};

// Static init.
AllocationStatsTracker::allocator_type::backing_allocator_type
    AllocationStatsTracker::m_allocator;

/**
 * @brief Records stats about an allocation.
 *
 * @param type_name Name of the type allocated.
 * @param file_name Source file from which the allocation call occurred.
 * @param line Source line from which the allocation call occurred.
 * @param bytes Number of bytes allocated.
 */
inline void AllocationStatsTracker::allocate(const char* type_name,
                                             const char* file_name,
                                             int line,
                                             size_t bytes) {
  AllocationStatsKey key{type_name, file_name, line};
  allocate(key, bytes);
}

/**
 * @brief Records stats about an allocation, identified by a key.
 *
 * @param key Call site from which the allocation call occurred.
 * @param bytes Number of bytes allocated.
 */
inline void AllocationStatsTracker::allocate(const AllocationStatsKey& key,
                                             size_t bytes) {
  m_stats[key].allocations++;
  m_stats[key].bytes_allocated += bytes;
}

/**
 * @brief Records stats about a deallocation.
 *
 * @param type_name Name of the type deallocated.
 * @param file_name Source file from which the original allocation call
 * occurred.
 * @param line Source line from which the original allocation call occurred.
 * @param bytes Number of bytes originally allocated.
 */
inline void AllocationStatsTracker::deallocate(const char* type_name,
                                               const char* file_name,
                                               int line,
                                               size_t bytes) {
  AllocationStatsKey key{type_name, file_name, line};
  deallocate(key, bytes);
}

/**
 * @brief Records stats about a deallocation, identified by a key.
 *
 * @param key Call site from which the original allocation call occurred.
 * @param bytes Number of bytes orignally allocated.
 */
inline void AllocationStatsTracker::deallocate(const AllocationStatsKey& key,
                                               size_t bytes) {
  m_stats[key].deallocations++;
  m_stats[key].bytes_deallocated += bytes;
}

inline const AllocationStats&
AllocationStatsTracker::stats(const AllocationStatsKey& key) const {
  auto stats = m_stats.find(key);
  if (stats == m_stats.end())
    return AllocationStats::null_stats;

  return stats->second;
}

} // namespace diagnostic
} // namespace allok8or