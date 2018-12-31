/**
 * @file diagnostic_allocation_stats_reporter-test.cpp
 * @brief Tests for the AllocationStatsReporter classes.
 *
 */

// My header.
#include "diagnostic/allocation_stats_reporter.h"

// Project headers.
#include "diagnostic/allocation_stats.h"

// Library headers.
#include "doctest.h"
#include <regex>
#include <sstream>
#include <string>

// Enable "safe" string functions.
#define __STDC_WANT_LIB_EXT1__ 1
#include <wchar.h> // std C

using namespace allok8or;

/**
 * @brief Counts the number of lines in the text input (newlines).
 *
 * @param text String containig newline-separated text.
 * @return int Number of newlines detected.
 */
int count_lines(std::string& text) {
  size_t pos = 0;
  int count = 0;
  for (; pos != std::string::npos;) {
    pos = text.find("\n", pos + 1);
    if (pos != std::string::npos) {
      ++count;
    }
  }

  return count;
}

/**
 * @brief Convenience wrapper for regex search.
 *
 * @param text Text to search./
 * @param pattern Regex pattern to search for.
 * @return true If pattern found.
 * @return false If pattern not found.
 */
bool check_regex(const std::string& text, const std::string& pattern) {
  std::regex re(pattern);
  return std::regex_search(text, re);
}

/**
 * @brief Verifies that the CSV header line is present.
 *
 * @param text CSV text to check.
 * @return true When the header line is correct.
 * @return false When the header line is not correct.
 */
bool check_header(const std::string& text) {
  return check_regex(
      text,
      std::string("TypeName,File,Line,Allocs,Alloc Bytes,Deallocs,Dealloc "
                  "Bytes,Net Allocs,Net Alloc Bytes"));
}

/**
 * @brief Verifies that expected numeric values appear in the text output.
 *
 * @param text Text to check.
 * @param stats An diagnostic::AllocationStats struct containing collected
 * stats.
 * @return true If found.
 * @return false If not found.
 */
bool check_line_values(const std::string& text,
                       const diagnostic::AllocationStats& stats) {
  char pat[128] = {'\0'};
  std::snprintf(pat,
                sizeof(pat),
                "%lld,%lld,%lld,%lld,%lld,%lld",
                stats.allocations,
                stats.bytes_allocated,
                stats.deallocations,
                stats.bytes_deallocated,
                stats.net_allocations(),
                stats.net_bytes());
  return check_regex(text, pat);
}

TEST_CASE("allocation_stats_csv_reporter") {
  diagnostic::AllocationStatsTracker tracker;
  diagnostic::AllocationStatsCsvReporter reporter;
  std::stringstream output;

  // Setup call sites.
  diagnostic::AllocationStatsKey foo{"Foo", "foo.h", 42};
  llong_t sizeof_foo = 492;

  diagnostic::AllocationStatsKey bar{"Bar", "bar.cpp", 420};
  llong_t sizeof_bar = 67;

  diagnostic::AllocationStatsKey baz{"Baz", "baz.hxx", 4200};
  llong_t sizeof_baz = 131;

  SUBCASE("allocate_none") {
    // No allocations

    reporter.report_stats(tracker.stats(), output);
    auto result = output.str();

    // Number of lines = 1 header only.
    CHECK_EQ(1, count_lines(result));
    CHECK_MESSAGE(check_header(result), "Invalid header in output.");

    // Should be no numeric values.
    CHECK_MESSAGE(false == check_regex(result, "^.*\\d.*$"),
                  "Found unexpected numeric values in output.");
  }

  SUBCASE("allocate_foo") {
    tracker.track_allocation(foo, sizeof_foo);
    tracker.track_allocation(foo, sizeof_foo);
    tracker.track_allocation(foo, sizeof_foo);

    reporter.report_stats(tracker.stats(), output);
    auto result = output.str();

    // Number of lines = 1 header + 1 data because only one type.
    CHECK_EQ(2, count_lines(result));
    CHECK_MESSAGE(check_header(result), "Invalid header in output.");

    // Check for known result values in the string.
    CHECK_MESSAGE(check_line_values(result, tracker.stats(foo)),
                  "No expected values found in output.");
  }

  SUBCASE("allocate_deallocate_foo") {
    tracker.track_allocation(foo, sizeof_foo);
    tracker.track_allocation(foo, sizeof_foo);
    tracker.track_allocation(foo, sizeof_foo);

    tracker.track_deallocation(foo, sizeof_foo);

    reporter.report_stats(tracker.stats(), output);
    auto result = output.str();

    // Number of lines = 1 header + 1 data because only one type.
    CHECK_EQ(2, count_lines(result));
    CHECK_MESSAGE(check_header(result), "Invalid header in output.");

    // Check for known result values in the string.
    CHECK_MESSAGE(check_line_values(result, tracker.stats(foo)),
                  "No expected values found in output.");
  }

  SUBCASE("allocate_multiple") {
    tracker.track_allocation(foo, sizeof_foo);
    tracker.track_allocation(foo, sizeof_foo);
    tracker.track_allocation(foo, sizeof_foo);

    tracker.track_allocation(bar, sizeof_bar);
    tracker.track_allocation(bar, sizeof_bar);

    tracker.track_allocation(baz, sizeof_baz);

    reporter.report_stats(tracker.stats(), output);
    auto result = output.str();

    // Number of lines = 1 header + 3 data because three types.
    CHECK_EQ(4, count_lines(result));
    CHECK_MESSAGE(check_header(result), "Invalid header in output.");

    // Check for known result values in the string.
    CHECK_MESSAGE(check_line_values(result, tracker.stats(foo)),
                  "No expected values found in output.");
    CHECK_MESSAGE(check_line_values(result, tracker.stats(bar)),
                  "No expected values found in output.");
    CHECK_MESSAGE(check_line_values(result, tracker.stats(baz)),
                  "No expected values found in output.");
  }

  SUBCASE("allocate_deallocate_multiple") {
    tracker.track_allocation(foo, sizeof_foo);
    tracker.track_allocation(foo, sizeof_foo);
    tracker.track_allocation(foo, sizeof_foo);
    tracker.track_deallocation(foo, sizeof_foo);

    tracker.track_allocation(bar, sizeof_bar);
    tracker.track_allocation(bar, sizeof_bar);

    tracker.track_allocation(baz, sizeof_baz);
    tracker.track_deallocation(baz, sizeof_baz);

    reporter.report_stats(tracker.stats(), output);
    auto result = output.str();

    // Number of lines = 1 header + 3 data because three types.
    CHECK_EQ(4, count_lines(result));
    CHECK_MESSAGE(check_header(result), "Invalid header in output.");

    // Check for known result values in the string.
    CHECK_MESSAGE(check_line_values(result, tracker.stats(foo)),
                  "No expected values found in output.");
    CHECK_MESSAGE(check_line_values(result, tracker.stats(bar)),
                  "No expected values found in output.");
    CHECK_MESSAGE(check_line_values(result, tracker.stats(baz)),
                  "No expected values found in output.");
  }
}
