/**
 * @file diagnostic_allocation_stats-test.cpp
 * @brief Unit tests of the AllocationStats* classes.
 *
 */

// My header.
#include "diagnostic_allocation_stats.h"

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