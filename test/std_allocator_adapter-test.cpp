/**
 * @file std_allocator_adapter-test.cpp
 * @brief Unit tests of the StdAllocatorAdapter class.
 */

// My header
#include "std_allocator_adapter.h"

// Project headers
#include "allocator.h"
#include "allocator_call_helper.h"
#include "diagnostic.h"
#include "pass_through.h"


// Library headers
#include "doctest.h"
#include <algorithm>
#include <array>
#include <map>
#include <string>

// Test data types.
template <typename T>
class FooT {
public:
  FooT(const std::string& name, const T& data) : m_data(data), m_name(name) {}
  T m_data;
  std::string m_name;
};

template <typename T, int N>
class FooNT {
public:
  FooNT() : m_data() {}
  std::array<T, N> m_data;

  constexpr bool operator<(const FooNT<T, N>& other) const {
    return m_data[0] < other.m_data[0];
  }
};

template <typename T>
class BarT : public FooNT<T, 7> {};

template <typename T>
bool operator==(const BarT<T>& lhs, const BarT<T>& rhs) {
  return lhs.m_data == rhs.m_data;
}

using namespace allok8or;

TEST_CASE("allocate") {
  PassThroughAllocator internal_allocator;
  StdAllocatorAdapter<BarT<double>, PassThroughAllocator> allocator(
      internal_allocator);

  auto memory = allocator.allocate(1); // one object only
  CHECK_NE(memory, nullptr);
  CHECK_EQ(sizeof(*memory), sizeof(BarT<double>));
}

TEST_CASE("allocate_array") {
  PassThroughAllocator internal_allocator;
  StdAllocatorAdapter<BarT<double>, PassThroughAllocator> allocator(
      internal_allocator);

  auto count = 32;
  auto memory = allocator.allocate(count);           // multiple objects
  CHECK_EQ(sizeof(memory[0]), sizeof(BarT<double>)); // TODO: better test?
}

TEST_CASE("deallocate") {
  PassThroughAllocator internal_allocator;
  StdAllocatorAdapter<BarT<double>, PassThroughAllocator> allocator(
      internal_allocator);

  auto memory = allocator.allocate(1); // one object only
  allocator.deallocate(memory, 0);
  CHECK_NE(memory, nullptr); // What to test, really? Make sure it links.
}

TEST_CASE("deallocate_array") {
  PassThroughAllocator internal_allocator;
  StdAllocatorAdapter<BarT<double>, PassThroughAllocator> allocator(
      internal_allocator);

  auto count = 32;
  auto memory = allocator.allocate(count); // multiple objects
  allocator.deallocate(memory, 0);
  CHECK_NE(memory, nullptr); // What to test, really? Make sure it links.
}

TEST_CASE("compare_with_pass_through") {
  // NOTE: PassThroughAllocator has no state, so all are equivalent.

  PassThroughAllocator internal_allocator1;
  StdAllocatorAdapter<BarT<double>, PassThroughAllocator> allocator1(
      internal_allocator1);

  PassThroughAllocator internal_allocator2;
  StdAllocatorAdapter<FooT<double>, PassThroughAllocator> allocator2(
      internal_allocator2);

  SUBCASE("same_instance_compares_equal") { CHECK_EQ(allocator1, allocator1); }

  SUBCASE("different_instances_compare_equal") {
    CHECK_EQ(allocator1, allocator2);
  }
}

TEST_CASE("std_map_with_pass_through") {
  PassThroughAllocator internal_allocator;
  using DoubleBar = BarT<double>;
  using TestMapValueType = std::pair<const int, DoubleBar>;
  using TestAllocator =
      StdAllocatorAdapter<TestMapValueType, PassThroughAllocator>;
  TestAllocator allocator(internal_allocator);

  std::map<int, DoubleBar, std::less<int>, TestAllocator> test_map(allocator);

  DoubleBar db1, db2, db3;
  db1.m_data.fill(0.42);
  db2.m_data.fill(42.0);
  db3.m_data.fill(0);

  test_map[0] = db1;
  test_map[42] = db2;
  test_map[4200] = db3;

  CHECK_EQ(3, test_map.size());

  CHECK_EQ(db1, test_map.find(0)->second);
  CHECK_EQ(db1, test_map[0]);

  CHECK_EQ(db2, test_map.find(42)->second);
  CHECK_EQ(db2, test_map[42]);

  CHECK_EQ(db3, test_map.find(4200)->second);
  CHECK_EQ(db3, test_map[4200]);

  CHECK_EQ(test_map.end(), test_map.find(7));
}

TEST_CASE("std_map_with_diagnostic") {
  using BackingAllocatorType = DiagnosticAllocator<PassThroughAllocator>;
  using DoubleBar = BarT<double>;
  using TestMapValueType = std::pair<const int, DoubleBar>;
  using TestAllocator =
      StdAllocatorAdapter<TestMapValueType, BackingAllocatorType>;
  
  PassThroughAllocator internal_allocator;
  BackingAllocatorType backing_allocator(internal_allocator);
  TestAllocator allocator(backing_allocator);

  std::map<int, DoubleBar, std::less<int>, TestAllocator> test_map(allocator);

  DoubleBar db1, db2, db3;
  db1.m_data.fill(0.42);
  db2.m_data.fill(42.0);
  db3.m_data.fill(0);

  test_map[0] = db1;
  test_map[42] = db2;
  test_map[4200] = db3;

  CHECK_EQ(3, test_map.size());

  CHECK_EQ(db1, test_map.find(0)->second);
  CHECK_EQ(db1, test_map[0]);

  CHECK_EQ(db2, test_map.find(42)->second);
  CHECK_EQ(db2, test_map[42]);

  CHECK_EQ(db3, test_map.find(4200)->second);
  CHECK_EQ(db3, test_map[4200]);

  CHECK_EQ(test_map.end(), test_map.find(7));
}