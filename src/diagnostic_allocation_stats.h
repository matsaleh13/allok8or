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
  llong_t allocations = 0;
  llong_t bytes_allocated = 0;

  llong_t deallocations = 0;
  llong_t bytes_deallocated = 0;
};

class AllocationStatsTracker {
public:
  void allocate(const char* type_name,
                const char* file_name,
                int line,
                size_t bytes) {
    AllocationStatsKey key{type_name, file_name, line};

    m_stats[key].allocations++;
    m_stats[key].bytes_allocated += bytes;
  }

  void deallocate(const char* type_name,
                  const char* file_name,
                  int line,
                  size_t bytes) {
    AllocationStatsKey key{type_name, file_name, line};

    m_stats[key].deallocations++;
    m_stats[key].bytes_deallocated += bytes;
  }

private:
  using value_type = std::pair<AllocationStatsKey, AllocationStats>;
  using allocator_type = StdAllocatorAdapter<value_type, PassThroughAllocator>;
  typedef std::unordered_map<AllocationStatsKey,
                             AllocationStats,
                             AllocationStatsKey::hash,
                             AllocationStatsKey::equal_to,
                             allocator_type>
      StatsMap;

  StatsMap m_stats;
};

} // namespace diagnostic
} // namespace allok8or