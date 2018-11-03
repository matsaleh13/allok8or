// My header
#include "pass_through.h"

// Project headers

// Library headers
#include <memory>
#include <stdlib.h>   // C11

namespace allok8or {

/**
 * Request memory from the system heap.
 */
void* PassThroughAllocator::allocate(
    size_t bytes, size_t alignment /*= alignof( std::max_align_t ) */) {
  auto memory = aligned_alloc(alignment, bytes);

  return memory;
}

/**
 * Returns memory to the system heap.
 */
void PassThroughAllocator::deallocate(void* memory) { std::free(memory); }

} // namespace allok8or
