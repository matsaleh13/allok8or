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
    size_t bytes, size_t alignment /*= alignof( std::max_align_t ) */) const {
  return memory::aligned_malloc(bytes, alignment);
}

/**
 * Returns memory to the system heap.
 */
void PassThroughAllocator::deallocate(void* memory) const {
  memory::aligned_free(memory);
}

} // namespace allok8or
