/**
 * @file diagnostic_allocation_stats-test.cpp
 * @brief Unit tests of the AllocationStats* classes.
 *
 */

// My header.
#include "diagnostic/allocation_stats.h"

// Project headers.

// Library headers.
#include "doctest.h"

using namespace allok8or;

//
// AllocationStatsKey tests
//

TEST_CASE("create_key") {
  auto type_name = "MySimpleType";
  auto file_name = "foo.cpp";
  int line = 42;

  diagnostic::AllocationStatsKey key{type_name, file_name, line};

  CHECK_EQ(type_name, key.type_name);
  CHECK_EQ(file_name, key.file_name);
  CHECK_EQ(line, key.line);
}

TEST_CASE("equal_keys_hash_equal") {
  auto type_name = "MySimpleType";
  auto file_name = "foo.cpp";
  int line = 42;

  diagnostic::AllocationStatsKey key1{type_name, file_name, line};
  diagnostic::AllocationStatsKey key2{type_name, file_name, line};

  auto h1 = diagnostic::AllocationStatsKey::hash()(key1);
  auto h2 = diagnostic::AllocationStatsKey::hash()(key2);
  CHECK_EQ(h1, h2);
}

TEST_CASE("different_keys_hash_unequal") {
  auto type_name = "MySimpleType";
  auto file_name = "foo.cpp";
  int line = 42;

  diagnostic::AllocationStatsKey key1{type_name, file_name, line};
  diagnostic::AllocationStatsKey key2{type_name, file_name, 43};

  auto h1 = diagnostic::AllocationStatsKey::hash()(key1);
  auto h2 = diagnostic::AllocationStatsKey::hash()(key2);
  CHECK_NE(h1, h2);
}

//
// AllocationStats tests
//
TEST_CASE("create_stats") {
  diagnostic::AllocationStats stats;

  CHECK_EQ(0, stats.allocations);
  CHECK_EQ(0, stats.bytes_allocated);
  CHECK_EQ(0, stats.deallocations);
  CHECK_EQ(0, stats.bytes_deallocated);
}

TEST_CASE("net_stats") {
  llong_t size = 42;
  llong_t allocs = 5;
  llong_t deallocs = 2;

  diagnostic::AllocationStats stats{
      allocs, allocs * size, deallocs, deallocs * size};

  CHECK_EQ(allocs, stats.allocations);
  CHECK_EQ(allocs * size, stats.bytes_allocated);
  CHECK_EQ(deallocs, stats.deallocations);
  CHECK_EQ(deallocs * size, stats.bytes_deallocated);
  CHECK_EQ(allocs - deallocs, stats.net_allocations());
  CHECK_EQ((allocs - deallocs) * size, stats.net_bytes());
}

//
// AllocationStatsTracker tests
//
TEST_CASE("create_tracker") {

  SUBCASE("on_stack") {
    diagnostic::AllocationStatsTracker tracker;

    CHECK_EQ(0, tracker.stats().size());
  }

  SUBCASE("via_allocator") {
    auto tracker = new diagnostic::AllocationStatsTracker();

    CHECK_NE(nullptr, tracker);
    CHECK_EQ(0, tracker->stats().size());

    delete tracker;
  }
}


TEST_CASE("track_allocations") {
  diagnostic::AllocationStatsTracker tracker;

  // Setup call sites.
  diagnostic::AllocationStatsKey foo{"Foo", "foo.h", 42};
  llong_t sizeof_foo = 492;
  llong_t foo_count = 0;

  diagnostic::AllocationStatsKey bar{"Bar", "bar.cpp", 420};
  llong_t sizeof_bar = 67;
  llong_t bar_count = 0;

  diagnostic::AllocationStatsKey baz{"Baz", "baz.hxx", 4200};
  llong_t sizeof_baz = 131;
  llong_t baz_count = 0;

  SUBCASE("allocate_foo") {
    tracker.track_allocation(foo, sizeof_foo);
    foo_count++;
    tracker.track_allocation(foo, sizeof_foo);
    foo_count++;
    tracker.track_allocation(foo, sizeof_foo);
    foo_count++;

    CHECK_EQ(foo_count, tracker.stats(foo).allocations);
    CHECK_EQ(foo_count * sizeof_foo, tracker.stats(foo).bytes_allocated);
  }

  SUBCASE("allocate_deallocate_foo") {
    tracker.track_allocation(foo, sizeof_foo);
    foo_count++;
    tracker.track_allocation(foo, sizeof_foo);
    foo_count++;
    tracker.track_allocation(foo, sizeof_foo);
    foo_count++;

    tracker.track_deallocation(foo, sizeof_foo);

    CHECK_EQ(foo_count, tracker.stats(foo).allocations);
    CHECK_EQ(foo_count * sizeof_foo, tracker.stats(foo).bytes_allocated);
    CHECK_EQ(1, tracker.stats(foo).deallocations);
    CHECK_EQ(1 * sizeof_foo, tracker.stats(foo).bytes_deallocated);
  }

  SUBCASE("allocate_multiple") {
    tracker.track_allocation(foo, sizeof_foo);
    foo_count++;
    tracker.track_allocation(foo, sizeof_foo);
    foo_count++;
    tracker.track_allocation(foo, sizeof_foo);
    foo_count++;

    tracker.track_allocation(bar, sizeof_bar);
    bar_count++;
    tracker.track_allocation(bar, sizeof_bar);
    bar_count++;

    tracker.track_allocation(baz, sizeof_baz);
    baz_count++;

    CHECK_EQ(foo_count, tracker.stats(foo).allocations);
    CHECK_EQ(foo_count * sizeof_foo, tracker.stats(foo).bytes_allocated);

    CHECK_EQ(bar_count, tracker.stats(bar).allocations);
    CHECK_EQ(bar_count * sizeof_bar, tracker.stats(bar).bytes_allocated);

    CHECK_EQ(baz_count, tracker.stats(baz).allocations);
    CHECK_EQ(baz_count * sizeof_baz, tracker.stats(baz).bytes_allocated);
  }

  SUBCASE("allocate_deallocate_multiple") {
    tracker.track_allocation(foo, sizeof_foo);
    foo_count++;
    tracker.track_allocation(foo, sizeof_foo);
    foo_count++;
    tracker.track_allocation(foo, sizeof_foo);
    foo_count++;
    tracker.track_deallocation(foo, sizeof_foo);
    foo_count--;

    tracker.track_allocation(bar, sizeof_bar);
    bar_count++;
    tracker.track_allocation(bar, sizeof_bar);
    bar_count++;

    tracker.track_allocation(baz, sizeof_baz);
    baz_count++;
    tracker.track_deallocation(baz, sizeof_baz);
    baz_count--;

    CHECK_EQ(foo_count, tracker.stats(foo).net_allocations());
    CHECK_EQ(foo_count * sizeof_foo, tracker.stats(foo).net_bytes());

    CHECK_EQ(bar_count, tracker.stats(bar).net_allocations());
    CHECK_EQ(bar_count * sizeof_bar, tracker.stats(bar).net_bytes());

    CHECK_EQ(baz_count, tracker.stats(baz).net_allocations());
    CHECK_EQ(baz_count * sizeof_baz, tracker.stats(baz).net_bytes());
  }

}
