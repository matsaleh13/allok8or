#pragma once

#include <cstddef>

namespace allok8or
{

/**
 * @brief Allocator that acts as a proxy to the system allocator.
 * 
 */
class PassThroughAllocator
{
public:
  PassThroughAllocator() {};
  ~PassThroughAllocator() {};

  // Public API
  void* allocate( size_t bytes, size_t alignment = alignof( std::max_align_t ) );
  void deallocate( void* page );
};

} // namespace allok8or
