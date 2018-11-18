/**
 * @file pass_through.h
 * @brief Header for a basic allocator that simply defers all allcations to the
 * system allocator.
 */
#pragma once

// Project headers
#include "allocator.h"

// Library headers
#include <cstddef>

namespace allok8or {

/**
 * @brief Allocator that acts as a proxy to the system allocator.
 *
 */
class PassThroughAllocator : public Allocator<PassThroughAllocator> {
public:
  constexpr PassThroughAllocator(){};
  ~PassThroughAllocator(){};

  // Public API
  void* allocate(size_t bytes, size_t alignment = alignof(std::max_align_t));
  void deallocate(void* page);
};

/**
 * @brief Equality test. Because PassThroughAllocator is stateless, all are equal.
 * 
 * @return true Always
 * @return false Never
 */
constexpr bool operator==(const PassThroughAllocator& lhs,
                          const PassThroughAllocator& rhs) {
  return true;
}

/**
 * @brief Inquality test. Because PassThroughAllocator is stateless, none are unqual.
 * 
 * @return true Never
 * @return false Always
 */
constexpr bool operator!=(const PassThroughAllocator& lhs,
                          const PassThroughAllocator& rhs) {
  return false;
}

} // namespace allok8or
