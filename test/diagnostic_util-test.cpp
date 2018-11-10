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
#include <regex>
#include <string>

using namespace allok8or;

//
// TypeNameHelper Tests
//

// Test data types.
template <typename T>
class FooT {};

template <typename T, int N>
struct FooTN {};

template <typename T>
class BarT : FooTN<BarT<T>, 7> {};

// Test cases
TEST_CASE("get_type_name_int") {
  auto type_name = diagnostic::get_type_name<int>();
  CHECK_EQ("int", std::string(type_name));

  // Static data, so verify second call.
  auto type_name2 = diagnostic::get_type_name<int>();
  CHECK_EQ(std::string(type_name2), std::string(type_name));
  CHECK_EQ(type_name2, type_name);  // ensure pointers are the same.
}

TEST_CASE("get_type_name_double") {
  auto type_name = diagnostic::get_type_name<double>();
  CHECK_EQ("double", std::string(type_name));

  // Static data, so verify second call.
  auto type_name2 = diagnostic::get_type_name<double>();
  CHECK_EQ(std::string(type_name2), std::string(type_name));
  CHECK_EQ(type_name2, type_name);  // ensure pointers are the same.
}

TEST_CASE("get_type_name_std_string") {
  auto type_name = diagnostic::get_type_name<std::string>();
  
  // For more complex types we need to match on regex because different
  // compilers emit different strings.
  std::regex pat("std::.*string");
  CHECK_UNARY(std::regex_search(type_name, pat));

  // Static data, so verify second call.
  auto type_name2 = diagnostic::get_type_name<std::string>();
  CHECK_EQ(std::string(type_name2), std::string(type_name));
  CHECK_EQ(type_name2, type_name);  // ensure pointers are the same.
}

TEST_CASE("get_type_name_FooT") {
  auto type_name = diagnostic::get_type_name<FooT<int>>();

  // For more complex types we need to match on regex because different
  // compilers emit different strings.
  std::regex pat("FooT\\<int\\>");
  CHECK_UNARY(std::regex_search(type_name, pat));

  // Static data, so verify second call.
  auto type_name2 = diagnostic::get_type_name<FooT<int>>();
  CHECK_EQ(std::string(type_name2), std::string(type_name));
  CHECK_EQ(type_name2, type_name);  // ensure pointers are the same.
}

TEST_CASE("get_type_name_FooTN") {
  auto type_name = diagnostic::get_type_name<FooTN<std::string, 42>>();

  // For more complex types we need to match on regex because different
  // compilers emit different strings.
  std::regex pat("FooTN\\<std::.*basic_string\\<char.*\\>, 42\\>");
  CHECK_UNARY(std::regex_search(type_name, pat));

  // Static data, so verify second call.
  auto type_name2 = diagnostic::get_type_name<FooTN<std::string, 42>>();
  CHECK_EQ(std::string(type_name2), std::string(type_name));
  CHECK_EQ(type_name2, type_name);  // ensure pointers are the same.
}

TEST_CASE("get_type_name_BarT") {
  auto type_name = diagnostic::get_type_name<BarT<double>>();

  // For more complex types we need to match on regex because different
  // compilers emit different strings.
  std::regex pat("BarT\\<double\\>");
  CHECK_UNARY(std::regex_search(type_name, pat));

  // Static data, so verify second call.
  auto type_name2 = diagnostic::get_type_name<BarT<double>>();
  CHECK_EQ(std::string(type_name2), std::string(type_name));
  CHECK_EQ(type_name2, type_name);  // ensure pointers are the same.
}
