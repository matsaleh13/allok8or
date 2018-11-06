// My header
#include "pass_through.h"

// Project headers
#include "memory.h"

// Library headers

namespace allok8or {

/**
 * Request memory from the system heap.
 */
void* PassThroughAllocator::allocate(
    size_t bytes, size_t alignment /*= alignof( std::max_align_t ) */) {
  auto memory = memory::aligned_malloc(bytes, alignment);

  return memory;
}

/**
 * Returns memory to the system heap.
 */
void PassThroughAllocator::deallocate(void* memory) {
  memory::aligned_free(memory);
}

} // namespace allok8or
