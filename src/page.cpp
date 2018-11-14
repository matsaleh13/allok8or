
// My header
#include "page.h"

// Project headers
#include "logging.h"
#include "align.h"

// Library headers
#include <memory>
#include <cassert>


namespace allok8or
{

/**
 * Header for an allocated page of memory.
 * Maintains an internal linked list for free pages. 
 * In-use pages are not tracked (at this time).
 */
struct PageHeader
{
  // Next page in the free list. Null if not free.
  PageHeader* next_page;

  // Valid states of the page.
  const static ulong_t FREE    = 'FREE';   // In the free list.
  const static ulong_t USED    = 'USED';   // Being used by an allocator.
  const static ulong_t DELETED = 'DLTD';   // Returned to the system heap.

  // The current state of the page: FREE, USED, or DELETED.
  ulong_t state;
  
  // Starting address of the memory returned to the calling allocator.
  void* user_data;
};

/**
 * Constructor.
 * @param page_size Size in bytes of the pages to be created by this allocator.
 * @param alignment Byte-alignment used in sizing these pages.
 */
PageAllocator::PageAllocator( size_t page_size, size_t alignment /*= alignof( std::max_align_t )*/ )
  : m_page_size( page_size )
  , m_alignment( alignment )
  , m_num_pages( 0 )
  , m_num_free_pages( 0 )
  , m_free_pages( nullptr )
{
}

/**
 * Destructor
 * Destroys all pages in the free page list. 
 * If any pages are left after cleanup, it's a memory leak.
 */
PageAllocator::~PageAllocator()
{
  cleanup();

  if( m_num_pages )
  {
    LOG_ERROR( "Page allocator deleted with allocated pages outstanding [%d], leaking [%d] bytes.", m_num_pages, m_num_pages * m_page_size );
  }
}

/**
 * Returns a free page from the free page list if available, otherwise allocates a new page from the heap.
 */
void* PageAllocator::allocate()
{
  if( !m_free_pages )
  {
    auto newPage = create();
    assert( newPage );
    if( !newPage )
    {
      // OOM
      LOG_ERROR( "Failed to allocate new page." );
      return nullptr;
    }

    add_page( newPage );
  }

  PageHeader* page = remove_page();
  if( page )
  {
    assert( !page->next_page );
    assert( page->state == PageHeader::USED );
    assert( page->user_data );

    return page->user_data;
  }

  assert( page );
  return nullptr;
}

/**
 * Returns the given page to the free page list.
 */
void PageAllocator::deallocate( void* page )
{
  PageHeader* header = reinterpret_cast<PageHeader*>(get_page_address( page ));

  assert( header );
  assert( header->user_data == page );
  assert( header->state == PageHeader::USED );

  add_page( header );
}

/**
 * Returns the aligned starting address of the given page's user data given the page header address.
 */
void* PageAllocator::get_aligned_user_data_address( void* page )
{
  auto unaligned_end = (void*) ( (uintptr_t) page + sizeof( PageHeader ) );
  return align::get_next_aligned_address( unaligned_end, m_alignment );
}

/**
 * Returns the starting address of the page header based on the given user data address.
 */
void* PageAllocator::get_page_address( void* user_data )
{
  auto unaligned_start = (void*) ( (uintptr_t) user_data - sizeof( PageHeader ) );
  return align::get_prev_aligned_address(unaligned_start, m_alignment);
}

/**
* Allocate a new page from the global heap and initialize it.
*/
allok8or::PageHeader* PageAllocator::create()
{
  PageHeader* newPage = reinterpret_cast<PageHeader*>( std::malloc( m_page_size ) );
  memset( newPage, 0, m_page_size );
  ++m_num_pages;

  newPage->user_data = get_aligned_user_data_address( newPage );

  return newPage;
}

/**
* Return a page to the global heap and mark it deleted.
*/
void PageAllocator::destroy( PageHeader* page )
{
  assert( page );

  page->state = PageHeader::DELETED;
  std::free( page );
  --m_num_pages;
}

/**
 * Add a page to the free page list.
 */
void PageAllocator::add_page( PageHeader* page )
{
  assert( page );
  assert( !page->next_page );
  assert( page->state != PageHeader::FREE );
  assert( verify_free_pages() );

  if( m_free_pages )
  {
    page->next_page = m_free_pages;
  }

  m_free_pages = page;
  page->state = PageHeader::FREE;

  ++m_num_free_pages;
}

/**
 * Remove a page from the free page list.
 */
PageHeader* PageAllocator::remove_page()
{
  assert( verify_free_pages() );

  auto page = m_free_pages;
  if( page )
  {
    m_free_pages = page->next_page;
    page->next_page = nullptr;
    page->state = PageHeader::USED;

    --m_num_free_pages;
  }

  return page;
}

/**
 * Remove all free pages and return them to the global heap.
 * This should be safe to call at any time.
 *
 * Returns the number of pages cleaned up.
 */
int PageAllocator::cleanup()
{
  PageHeader* page = nullptr;
  int count = 0;
  do
  {
    page = remove_page();
    if (page)
    {
      assert( page->state != PageHeader::FREE );
      destroy( page );
      ++count;
    }
  }
  while( page );

  return count;
}


}  // namespace allok8or
