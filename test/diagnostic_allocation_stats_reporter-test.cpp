/**
 * @file diagnostic_allocation_stats_reporter-test.cpp
 * @brief Tests for the AllocationStatsReporter classes.
 *
 */

// My header.
#include "diagnostic_allocation_stats_reporter.h"

// Project headers.
#include "diagnostic_allocation_stats.h"

// Library headers.
#include "doctest.h"
#include <regex>
#include <string>
#include <sstream>

using namespace allok8or;

TEST_CASE("allocation_stats_reporter") {
  diagnostic::AllocationStatsTracker tracker;
  diagnostic::AllocationStatsCsvReporter reporter;
  std::stringstream output;

  // Setup call sites.
  diagnostic::AllocationStatsKey foo{"Foo", "foo.h", 42};
  llong_t sizeof_foo = 492;
  llong_t foo_count = 0;

  // diagnostic::AllocationStatsKey bar{"Bar", "bar.cpp", 420};
  // llong_t sizeof_bar = 67;
  // llong_t bar_count = 0;

  // diagnostic::AllocationStatsKey baz{"Baz", "baz.hxx", 4200};
  // llong_t sizeof_baz = 131;
  // llong_t baz_count = 0;

  SUBCASE("allocate_foo") {
    tracker.track_allocation(foo, sizeof_foo);
    foo_count++;
    tracker.track_allocation(foo, sizeof_foo);
    foo_count++;
    tracker.track_allocation(foo, sizeof_foo);
    foo_count++;

    reporter.report_stats(tracker.stats(), output);
    auto result = output.str();

    // Number of lines = 1 header + 1 data
    size_t pos = 0;
    int count = 0;
    for ( ; pos != std::string::npos; ) {
      pos = result.find("\n", pos+1);
      if (pos != std::string::npos) {
        ++count;
      }
    }

    CHECK_EQ(2, count);

    // std::string pat("std::.*string");
    // std::regex re(pat);
    // CHECK_MESSAGE(std::regex_search(type_name, re), pat);
  }

  // SUBCASE("allocate_deallocate_foo") {
  //   tracker.track_allocation(foo, sizeof_foo);
  //   foo_count++;
  //   tracker.track_allocation(foo, sizeof_foo);
  //   foo_count++;
  //   tracker.track_allocation(foo, sizeof_foo);
  //   foo_count++;

  //   tracker.track_deallocation(foo, sizeof_foo);

  //   CHECK_EQ(foo_count, tracker.stats(foo).allocations);
  //   CHECK_EQ(foo_count * sizeof_foo, tracker.stats(foo).bytes_allocated);
  //   CHECK_EQ(1, tracker.stats(foo).deallocations);
  //   CHECK_EQ(1 * sizeof_foo, tracker.stats(foo).bytes_deallocated);
  // }

  // SUBCASE("allocate_multiple") {
  //   tracker.track_allocation(foo, sizeof_foo);
  //   foo_count++;
  //   tracker.track_allocation(foo, sizeof_foo);
  //   foo_count++;
  //   tracker.track_allocation(foo, sizeof_foo);
  //   foo_count++;

  //   tracker.track_allocation(bar, sizeof_bar);
  //   bar_count++;
  //   tracker.track_allocation(bar, sizeof_bar);
  //   bar_count++;

  //   tracker.track_allocation(baz, sizeof_baz);
  //   baz_count++;

  //   CHECK_EQ(foo_count, tracker.stats(foo).allocations);
  //   CHECK_EQ(foo_count * sizeof_foo, tracker.stats(foo).bytes_allocated);

  //   CHECK_EQ(bar_count, tracker.stats(bar).allocations);
  //   CHECK_EQ(bar_count * sizeof_bar, tracker.stats(bar).bytes_allocated);

  //   CHECK_EQ(baz_count, tracker.stats(baz).allocations);
  //   CHECK_EQ(baz_count * sizeof_baz, tracker.stats(baz).bytes_allocated);
  // }

  // SUBCASE("allocate_deallocate_multiple") {
  //   tracker.track_allocation(foo, sizeof_foo);
  //   foo_count++;
  //   tracker.track_allocation(foo, sizeof_foo);
  //   foo_count++;
  //   tracker.track_allocation(foo, sizeof_foo);
  //   foo_count++;
  //   tracker.track_deallocation(foo, sizeof_foo);
  //   foo_count--;

  //   tracker.track_allocation(bar, sizeof_bar);
  //   bar_count++;
  //   tracker.track_allocation(bar, sizeof_bar);
  //   bar_count++;

  //   tracker.track_allocation(baz, sizeof_baz);
  //   baz_count++;
  //   tracker.track_deallocation(baz, sizeof_baz);
  //   baz_count--;

  //   CHECK_EQ(foo_count, tracker.stats(foo).net_allocations());
  //   CHECK_EQ(foo_count * sizeof_foo, tracker.stats(foo).net_bytes());

  //   CHECK_EQ(bar_count, tracker.stats(bar).net_allocations());
  //   CHECK_EQ(bar_count * sizeof_bar, tracker.stats(bar).net_bytes());

  //   CHECK_EQ(baz_count, tracker.stats(baz).net_allocations());
  //   CHECK_EQ(baz_count * sizeof_baz, tracker.stats(baz).net_bytes());
  // }
}
