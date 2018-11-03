#include "doctest.h"
#include "pass_through.h"

#include <memory>


TEST_CASE( "allocate" )
{
  allok8or::PassThroughAllocator allocator;

  void* memory = allocator.allocate(1024);
  CHECK_NE( memory, nullptr );
}


TEST_CASE( "deallocate" )
{
  allok8or::PassThroughAllocator allocator;

  void* memory = allocator.allocate(1024);
  allocator.deallocate( memory );

  // What to test, really? Make sure it links.
}