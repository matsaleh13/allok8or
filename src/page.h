#pragma once

#include "types.h"

#include <cstddef>

namespace allok8or
{

struct PageHeader;

/**
 * Responsible for requesting memory pages from the system heap so that 
 * other allocator types can use them as backing memory. 
 *
 * TODO: Not thread safe.
 */
class PageAllocator
{
public:
  PageAllocator( size_t page_size, size_t alignment = alignof( std::max_align_t ) );
  ~PageAllocator();

  PageAllocator() = delete;
  PageAllocator( const PageAllocator& ) = delete;
  PageAllocator( const PageAllocator&& ) = delete;

  // Public API
  void* allocate();
  void deallocate( void* page );

  int cleanup();

  size_t page_size();
  int num_pages();
  int num_free_pages();

private:
  void* get_aligned_user_data_address( void* page );
  void* get_page_address( void* user_data );
  bool verify_free_pages();

  PageHeader* create();
  void destroy( PageHeader* page );

  void add_page( PageHeader* page );
  PageHeader* remove_page();

  const size_t m_page_size;
  const size_t m_alignment;
  int m_num_pages;

  int m_num_free_pages;
  PageHeader* m_free_pages;
};

inline
size_t PageAllocator::page_size()
{
  return m_page_size;
}

inline
int PageAllocator::num_pages()
{
  return m_num_pages;
}

inline
int PageAllocator::num_free_pages()
{
  return m_num_free_pages;
}

inline
bool PageAllocator::verify_free_pages()
{
  return ( (m_free_pages && ( m_num_free_pages > 0 )) || ( !m_free_pages && m_num_free_pages == 0 ) );
}


} // namespace Allocators

