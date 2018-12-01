/**
 * @file diagnostic_allocation_stats_reporter.h
 * @brief Aggregates and reports information about allocations.
 *
 */
#pragma once

// Project headers.
#include "diagnostic_allocation_stats.h"

// Library headers.

namespace allok8tor {
namespace diagnostic {

class AllocationStatsReporter {
public:
  constexpr static void report_stats(const AllocationStatsTracker& stats);

private:

};

inline
constexpr void AllocationStatsReporter::report_stats(const AllocationStatsTracker& stats) {
  
}


} // namespace diagnostic
} // namespace allok8tor
