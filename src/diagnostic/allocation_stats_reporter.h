/**
 * @file diagnostic_allocation_stats_reporter.h
 * @brief Aggregates and reports information about allocations.
 *
 */
#pragma once

// Project headers.
#include "allocation_stats.h"

// Library headers.
#include <iostream>

namespace allok8or {
namespace diagnostic {

/**
 * @brief Base class for reporting stats to an output stream.
 *
 * @tparam TImplementation Implementation class for specific output formats.
 */
template <typename TImplementation>
class AllocationStatsReporter {
public:
  constexpr void report_stats(const AllocationStatsTracker::StatsMap& stats,
                              std::ostream& output) const {
    impl().report_stats(stats, output);
  }

protected:
  AllocationStatsReporter() {} // Don't create the base class.

private:
  constexpr TImplementation& impl() {
    return *static_cast<TImplementation*>(this);
  }

  constexpr const TImplementation& impl() const {
    return *static_cast<const TImplementation*>(this);
  }
};

/**
 * @brief AllocationStatsReporter that formats stats using comma-separated values (CSV).
 * 
 */
class AllocationStatsCsvReporter
    : public AllocationStatsReporter<AllocationStatsCsvReporter> {
public:
  void report_stats(const AllocationStatsTracker::StatsMap& stats,
                              std::ostream& output) const;
};

/**
 * @brief Dump stats to output stream.
 * 
 * @param stats An AllocationTracker::StatsMap contiaining allocation stats.
 * @param output A std::ostream to collect the output.
 */
inline void AllocationStatsCsvReporter::report_stats(
    const AllocationStatsTracker::StatsMap& stats, std::ostream& output) const {
      // Header row (column names)
      output << "TypeName" << "," << "File" << "," << "Line" << ","
             << "Allocs" << "," << "Alloc Bytes" << ","
             << "Deallocs" << "," << "Dealloc Bytes" << ","
             << "Net Allocs" << "," 
             << "Net Alloc Bytes" << "\n";
             
  for (const auto& stat_pair : stats) {
    output << stat_pair.first.type_name << ","
           << stat_pair.first.file_name << ","
           << stat_pair.first.line << ","
           << stat_pair.second.allocations << ","
           << stat_pair.second.bytes_allocated << ","
           << stat_pair.second.deallocations << ","
           << stat_pair.second.bytes_deallocated << ","
           << stat_pair.second.net_allocations() << ","
           << stat_pair.second.net_bytes() << "\n";
  }

  output << std::flush;
}

} // namespace diagnostic
} // namespace allok8or
