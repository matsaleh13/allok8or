#include "allocator.h"
#include "doctest.h"
#include "pass_through.h"

/**
 * @brief Wrapper to call through base class interface.
 */
template <typename TAlloc>
void* call_allocate(allok8or::Allocator<TAlloc>& a,
                    size_t size,
                    size_t alignment) {
  return a.allocate(size, alignment);
}

/**
 * @brief Wrapper to call through base class interface.
 */
template <typename TAlloc>
void call_deallocate(allok8or::Allocator<TAlloc>& a, void* data) {
  a.deallocate(data);
}

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