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
  void* result;
#ifdef _MSC_VER
  result = _aligned_malloc(size, align);
#else
  if (posix_memalign(&result, align, size)) {
    result = 0;
  }
#endif
  return result;
}

void aligned_free(void* ptr) {
#ifdef _MSC_VER
  _aligned_free(ptr);
#else
  free(ptr);
#endif
}
} // namespace memory
} // namespace allok8or
