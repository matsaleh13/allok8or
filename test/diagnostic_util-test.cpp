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
}

TEST_CASE("get_type_name_double") {
  auto type_name = diagnostic::get_type_name<double>();
  CHECK_EQ("double", std::string(type_name));
}

TEST_CASE("get_type_name_FooT") {
  auto type_name = diagnostic::get_type_name<FooT<int>>();
  CHECK_EQ("class FooT<int>", std::string(type_name));
}

TEST_CASE("get_type_name_FooTN") {
  auto type_name = diagnostic::get_type_name<FooTN<std::string, 42>>();
  CHECK_EQ("struct FooTN<class std::basic_string<char,struct "
           "std::char_traits<char>,class std::allocator<char> >,42>",
           std::string(type_name));
}

TEST_CASE("get_type_name_BarT") {
  auto type_name = diagnostic::get_type_name<BarT<double>>();
  CHECK_EQ("class BarT<double>", std::string(type_name));
}

