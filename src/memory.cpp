/**
 * @file memory.cpp
 * @brief Cross-platform memory-related functions.
 *
 */

#include "memory.h"

#ifdef _MSC_VER
#include <malloc.h>
#else
#include <stdlib.h>
#endif

namespace allok8or {
namespace memory {

void* aligned_malloc(size_t size, size_t align) {
  void* memory;
#ifdef _MSC_VER
  memory = _aligned_malloc(size, align);
#else
  if (posix_memalign(&memory, align, size)) {
    memory = nullptr;
  }
#endif
  return memory;
}

void aligned_free(void* memory) {
#ifdef _MSC_VER
  _aligned_free(memory);
#else
  free(memory);
#endif
}
} // namespace memory
} // namespace allok8or
