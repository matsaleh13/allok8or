/**
 * @file global_allocator-test.cpp
 * @brief Unit tests of the GlobalAllocator class.
 *
 */
// My header
#include "global.h"

// Project headers
#include "allocator.h"
#include "pass_through.h"

// Library headers
#include "doctest.h"
#include <memory>

using namespace allok8or;

//
// Test allocator class
//

using TestAllocator = PassThroughAllocator;
using Global = global::GlobalAllocator<TestAllocator>;

TEST_CASE("global_allocator") {
  const std::unique_ptr<TestAllocator> allocator =
      std::make_unique<TestAllocator>();

  SUBCASE("init") {
    Global::init(allocator.get());
    CHECK_EQ(allocator.get(), Global::get());
  }

  SUBCASE("cleanup") {
    Global::init(allocator.get());
    Global::cleanup();
    CHECK_EQ(nullptr, Global::get());
  }

  SUBCASE("") {
    Global::init(allocator.get());

    SUBCASE("allocate") {
      auto memory = Global::allocate(1024);
      CHECK_NE(memory, nullptr);
    }

    SUBCASE("allocate_aligned") {
      auto memory = Global::allocate(1024, 8);
      CHECK_NE(memory, nullptr);
    }

    SUBCASE("deallocate") {
      auto memory = Global::allocate(1024);
      Global::deallocate(memory);

      // What to test, really? Make sure it links.
    }
  }
}
