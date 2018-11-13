/**
 * @file diagnostic_util-test.cpp
 * @brief Unit tests of the diagnostic utility classes.
 *
 */

// My header
#include "diagnostic_util.h"

// Project headers

// Library headers
#include "doctest.h"

using namespace allok8or;

TEST_CASE("caller_details") {
  auto file_name = __FILE__;
  auto line = __LINE__;

  auto details = diagnostic::CallerDetails(file_name, line);

  CHECK_EQ(file_name, details.file_name());
  CHECK_EQ(line, details.line());
}