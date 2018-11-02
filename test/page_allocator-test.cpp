#include "doctest.h"

#include "page.h"

static const size_t DEFAULT_PAGE_SIZE = 1024;

TEST_CASE("create_page_allocator") {
  allok8or::PageAllocator allocator(DEFAULT_PAGE_SIZE);

  CHECK_EQ(allocator.page_size(), DEFAULT_PAGE_SIZE);
  CHECK_EQ(allocator.num_free_pages(), 0);
  CHECK_EQ(allocator.num_pages(), 0);
}

TEST_CASE( "allocate_page" )
{
  allok8or::PageAllocator allocator( DEFAULT_PAGE_SIZE );

  void* page = allocator.allocate();
  CHECK_NE( page, nullptr );
  CHECK_EQ( allocator.num_pages(), 1 );
  CHECK_EQ( allocator.num_free_pages(), 0 );
}

TEST_CASE( "allocate_several_pages" )
{
  allok8or::PageAllocator allocator( DEFAULT_PAGE_SIZE );

  allocator.allocate();
  allocator.allocate();
  allocator.allocate();

  CHECK_EQ( allocator.num_pages(), 3 );
  CHECK_EQ( allocator.num_free_pages(), 0 );
}

TEST_CASE( "deallocate_page" )
{
  allok8or::PageAllocator allocator( DEFAULT_PAGE_SIZE );

  void* page = allocator.allocate();

  allocator.deallocate( page );

  CHECK_EQ( allocator.num_pages(), 1 );
  CHECK_EQ( allocator.num_free_pages(), 1 );
}

TEST_CASE( "deallocate_several_pages" )
{
  allok8or::PageAllocator allocator( DEFAULT_PAGE_SIZE );

  void* page1 = allocator.allocate();
  void* page2 = allocator.allocate();
  void* page3 = allocator.allocate();

  allocator.deallocate( page1 );
  allocator.deallocate( page2 );
  allocator.deallocate( page3 );

  CHECK_EQ( allocator.num_pages(), 3 );
  CHECK_EQ( allocator.num_free_pages(), 3 );
}

TEST_CASE( "allocate_after_deallocate_page" )
{
  allok8or::PageAllocator allocator( DEFAULT_PAGE_SIZE );

  void* page1 = allocator.allocate();

  allocator.deallocate( page1 );

  void* page2 = allocator.allocate();
  CHECK_EQ( page1, page2 );

  CHECK_EQ( allocator.num_pages(), 1 );
  CHECK_EQ( allocator.num_free_pages(), 0 );
}

TEST_CASE( "allocate_after_deallocate_several_pages" )
{
  allok8or::PageAllocator allocator( DEFAULT_PAGE_SIZE );

  allocator.allocate();
  void* page2 = allocator.allocate();
  void* page3 = allocator.allocate();

  allocator.deallocate( page2 );
  allocator.deallocate( page3 );

  void* page3b = allocator.allocate();
  void* page2b = allocator.allocate();

  CHECK_EQ( page3, page3b );
  CHECK_EQ( page2, page2b );

  CHECK_EQ( allocator.num_pages(), 3 );
  CHECK_EQ( allocator.num_free_pages(), 0 );
}

TEST_CASE( "cleanup_when_no_pages_free" )
{
  allok8or::PageAllocator allocator( DEFAULT_PAGE_SIZE );

  allocator.allocate();
  allocator.allocate();
  allocator.allocate();

  auto cleanedCount = allocator.cleanup();

  CHECK_EQ( cleanedCount, 0 );
  CHECK_EQ( allocator.num_pages(), 3 );
  CHECK_EQ( allocator.num_free_pages(), 0 );
}

TEST_CASE( "cleanup_when_several_pages_free" )
{
  allok8or::PageAllocator allocator( DEFAULT_PAGE_SIZE );

  void* page1 = allocator.allocate();
  allocator.allocate();
  allocator.allocate();

  allocator.deallocate( page1 );

  auto cleanedCount = allocator.cleanup();

  CHECK_EQ( cleanedCount, 1 );
  CHECK_EQ( allocator.num_pages(), 2 );
  CHECK_EQ( allocator.num_free_pages(), 0 );
}

TEST_CASE( "cleanup_when_all_pages_free" )
{
  allok8or::PageAllocator allocator( DEFAULT_PAGE_SIZE );

  void* page1 = allocator.allocate();
  void* page2 = allocator.allocate();
  void* page3 = allocator.allocate();

  allocator.deallocate( page1 );
  allocator.deallocate( page2 );
  allocator.deallocate( page3 );

  auto cleanedCount = allocator.cleanup();

  CHECK_EQ( cleanedCount, 3 );
  CHECK_EQ( allocator.num_pages(), 0 );
  CHECK_EQ( allocator.num_free_pages(), 0 );
}