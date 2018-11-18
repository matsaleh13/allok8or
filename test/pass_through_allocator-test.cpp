/**
 * @file pass_through_allocator-test.cpp
 * @brief 
 */

// My header
#include "pass_through.h"

// Project headers
#include "allocator_call_helper.h"
#include "allocator.h"

// Library headers
#include "doctest.h"


TEST_CASE("allocate") {
  allok8or::PassThroughAllocator allocator;

  // Call through class type.
  void* memory = allocator.allocate(1024);
  CHECK_NE(memory, nullptr);

  // Call through the interface type.
  memory = call_allocate(allocator, 1024, 8);
  CHECK_NE(memory, nullptr);
}

TEST_CASE("deallocate") {
  allok8or::PassThroughAllocator allocator;

  // Call through class type.
  void* memory = allocator.allocate(1024);
  allocator.deallocate(memory);

  // Call through the interface type.
  memory = call_allocate(allocator, 1024, 8);
  call_deallocate(allocator, memory);
  CHECK_NE(memory, nullptr);

  // What to test, really? Make sure it links.
}

TEST_CASE("compare_equal") {
  allok8or::PassThroughAllocator allocator1 ;
  allok8or::PassThroughAllocator allocator2;

  SUBCASE("same_instance_compares_equal") {
    CHECK_EQ(allocator1, allocator1);
  }

  SUBCASE("different_instances_compare_equal") {
    CHECK_EQ(allocator1, allocator2);
  }
}
