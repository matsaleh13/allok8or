/**
 * @file std_allocator_adapter-test.cpp
 * @brief Unit tests of the StdAllocatorAdapter class.
 */

// My header
#include "std_allocator_adapter.h"

// Project headers
#include "allocator.h"
#include "diagnostic.h"
#include "mock_allocator.h"


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
  bool allocate_called = false;
  auto on_allocate = [&](size_t, size_t) { allocate_called = true; };

  test::MockAllocator internal_allocator(on_allocate);
  StdAllocatorAdapter<BarT<double>, test::MockAllocator> allocator(
      internal_allocator);

  auto memory = allocator.allocate(1); // one object only
  CHECK_MESSAGE(allocate_called, "allocate not called");
  CHECK_NE(memory, nullptr);
  CHECK_EQ(sizeof(*memory), sizeof(BarT<double>));
}

TEST_CASE("allocate_array") {
  bool allocate_called = false;
  auto on_allocate = [&](size_t, size_t) { allocate_called = true; };

  test::MockAllocator internal_allocator(on_allocate);
  StdAllocatorAdapter<BarT<double>, test::MockAllocator> allocator(
      internal_allocator);

  auto count = 32;
  auto memory = allocator.allocate(count);           // multiple objects

  CHECK_MESSAGE(allocate_called, "allocate not called");
  CHECK_EQ(sizeof(memory[0]), sizeof(BarT<double>)); // TODO: better test?
}

TEST_CASE("deallocate") {
  bool deallocate_called = false;
  auto on_deallocate = [&](void*) { deallocate_called = true; };

  test::MockAllocator internal_allocator(nullptr, on_deallocate);
  StdAllocatorAdapter<BarT<double>, test::MockAllocator> allocator(
      internal_allocator);

  auto memory = allocator.allocate(1); // one object only
  allocator.deallocate(memory, 0);
  CHECK_MESSAGE(deallocate_called, "deallocate not called");
  CHECK_NE(memory, nullptr); // What to test, really? Make sure it links.
}

TEST_CASE("deallocate_array") {
  bool deallocate_called = false;
  auto on_deallocate = [&](void*) { deallocate_called = true; };

  test::MockAllocator internal_allocator(nullptr, on_deallocate);
  StdAllocatorAdapter<BarT<double>, test::MockAllocator> allocator(
      internal_allocator);

  auto count = 32;
  auto memory = allocator.allocate(count); // multiple objects
  allocator.deallocate(memory, 0);
  CHECK_MESSAGE(deallocate_called, "deallocate not called");
  CHECK_NE(memory, nullptr); // What to test, really? Make sure it links.
}

TEST_CASE("compare_with_pass_through") {
  // NOTE: test::MockAllocator has no state, so all are equivalent.

  test::MockAllocator internal_allocator1;
  StdAllocatorAdapter<BarT<double>, test::MockAllocator> allocator1(
      internal_allocator1);

  test::MockAllocator internal_allocator2;
  StdAllocatorAdapter<FooT<double>, test::MockAllocator> allocator2(
      internal_allocator2);

  SUBCASE("same_instance_compares_equal") { CHECK_EQ(allocator1, allocator1); }

  SUBCASE("different_instances_compare_equal") {
    CHECK_EQ(allocator1, allocator2);
  }
}

TEST_CASE("std_map_add_remove") {
  int allocate_called = 0;
  auto on_allocate = [&](size_t, size_t) { ++allocate_called; };

  int deallocate_called = 0;
  auto on_deallocate = [&](void*) { ++deallocate_called; };

  test::MockAllocator internal_allocator(on_allocate, on_deallocate);
  using DoubleBar = BarT<double>;
  using TestMapValueType = std::pair<const int, DoubleBar>;
  using TestAllocator =
      StdAllocatorAdapter<TestMapValueType, test::MockAllocator>;
  TestAllocator allocator(internal_allocator);

  std::map<int, DoubleBar, std::less<int>, TestAllocator> test_map(allocator);
  allocate_called = 0;  // Reset, because some allocations may be called at creation

  DoubleBar db1, db2, db3;
  db1.m_data.fill(0.42);
  db2.m_data.fill(42.0);
  db3.m_data.fill(0);

  test_map[0] = db1;
  CHECK_EQ(1, allocate_called);

  test_map[42] = db2;
  CHECK_EQ(2, allocate_called);

  test_map[4200] = db3;
  CHECK_EQ(3, allocate_called);

  CHECK_EQ(3, test_map.size());

  CHECK_EQ(db1, test_map.find(0)->second);
  CHECK_EQ(db1, test_map[0]);

  CHECK_EQ(db2, test_map.find(42)->second);
  CHECK_EQ(db2, test_map[42]);

  CHECK_EQ(db3, test_map.find(4200)->second);
  CHECK_EQ(db3, test_map[4200]);

  test_map.erase(42);
  CHECK_EQ(1, deallocate_called);

  CHECK_EQ(test_map.end(), test_map.find(7));
}

TEST_CASE("std_map_add_remove_with_copy") {
  int allocate_called = 0;
  auto on_allocate = [&](size_t, size_t) { ++allocate_called; };

  int deallocate_called = 0;
  auto on_deallocate = [&](void*) { ++deallocate_called; };

  test::MockAllocator internal_allocator(on_allocate, on_deallocate);
  using DoubleBar = BarT<double>;
  using TestMapValueType = std::pair<const int, DoubleBar>;
  using TestAllocator =
      StdAllocatorAdapter<TestMapValueType, test::MockAllocator>;
  TestAllocator allocator(internal_allocator);
  using TestMap = std::map<int, DoubleBar, std::less<int>, TestAllocator>;

  TestMap test_map(allocator);

  DoubleBar db1, db2, db3;
  db1.m_data.fill(0.42);
  db2.m_data.fill(42.0);
  db3.m_data.fill(0);

  test_map[0] = db1;
  test_map[42] = db2;
  test_map[4200] = db3;

  // copy the map
  TestMap test_map_copy(test_map);

  // StdAllocatorAdapters should be equal.
  CHECK_EQ(test_map.get_allocator(), test_map_copy.get_allocator());
 
  // Backing allocator should be shared.
  CHECK_EQ(&(test_map.get_allocator().allocator()), &(test_map_copy.get_allocator().allocator()));

  // Contents preserved.
  CHECK_EQ(3, test_map_copy.size());

  CHECK_EQ(db1, test_map_copy.find(0)->second);
  CHECK_EQ(db1, test_map_copy[0]);

  CHECK_EQ(db2, test_map_copy.find(42)->second);
  CHECK_EQ(db2, test_map_copy[42]);

  CHECK_EQ(db3, test_map_copy.find(4200)->second);
  CHECK_EQ(db3, test_map_copy[4200]);

  CHECK_EQ(test_map_copy.end(), test_map_copy.find(7));

  // Now change the copy.
  allocate_called = 0;  // Reset
  test_map_copy[42000] = db1;
  CHECK_EQ(1, allocate_called);

}

