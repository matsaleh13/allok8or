/**
 * @file diagnostic.h
 * @brief Header file for the DiagnosticAllocator, which tracks and reports on allocations.
 * 
 */
#pragma once

// Project headers


// Library headers
#include <cstddef>



namespace allok8or
{

/**
* DiagnosticAllocator
*
* An allocator that tracks and reports on net allocations for the purpose of 
* identifying memory leaks.
*
* This allocator does not itself manage allocated memory. It passes that responsibility
* off to another allocator that is passed as an argument.
*/
template <typename AllocatorT>
class DiagnosticAllocator
{
public:
  DiagnosticAllocator( AllocatorT& );
  ~DiagnosticAllocator() {};

  void* allocate( size_t size, size_t alignment );

  void deallocate( void* user_data );

  const Tracking::AllocationTracker& Tracker() const { return m_tracker; }

private:

  AllocatorT& m_allocator;
  Tracking::AllocationTracker m_tracker;
};


/**
 * Constructor
 * @param pAllocator Reference to the allocator that will do the actual memory allocation.
 */
template <typename AllocatorT>
DiagnosticAllocator<AllocatorT>::DiagnosticAllocator( AllocatorT& pAllocator )
  : m_allocator(pAllocator)
{
}


/**
* allocate
* Acquires a block of memory from the backing allocator large enough to hold user data of size size, plus
* an additional header for tracking purposes. Returns only the pointer to the user region of the
* memory block, hiding the header from the caller.
*
* @param pUserDataSize The size required to hold the user data to be allocated.
* @param pUserDataAlignment The alignment to use for the allocation.
* @return Pointer to the user region of the allocated memory.
*/
template <typename AllocatorT>
void* allok8or::DiagnosticAllocator<AllocatorT>::allocate( size_t pUserDataSize, size_t pUserDataAlignment )
{
  auto alignedUserBytes = AlignUtil::GetAlignedSize( pUserDataSize, pUserDataAlignment );
  auto totalBytes = alignedUserBytes + AlignUtil::GetAlignedSize( sizeof( Tracking::TwnBlockHeader ), alignof( Tracking::TwnBlockHeader ) );

  auto memory = m_allocator.allocate( totalBytes, pUserDataAlignment );
  auto header = Tracking::TwnBlockHeader::create( memory, alignedUserBytes, pUserDataAlignment );

  m_tracker.add_block( header );

  return header->user_data();
}

//
///**
//* allocate
//* Acquires a block of memory from the backing allocator large enough to hold an instance of type T, plus
//* an additional header for tracking purposes. Returns only the pointer to the user region of the
//* memory block, hiding the header from the caller.
//*
//* @param pUserDataAlignment The alignment to use for the allocation.
//* @return Pointer to allocated memory.
//*/
//template <typename AllocatorT>
//template <typename AllocT>
//void* DiagnosticAllocator<AllocatorT>::allocate( const char* pFileName, int pLine, size_t pUserDataAlignment /*= alignof( AllocT ) */ )
//{
//  auto typeName = Tracking::get_type_name<AllocT>();
//  auto alignedUserBytes = AlignUtil::GetAlignedSize( sizeof( AllocT ), pUserDataAlignment );
//  auto totalBytes = alignedUserBytes + AlignUtil::GetAlignedSize( sizeof( Tracking::TwnBlockHeader ), alignof( Tracking::TwnBlockHeader ) );
//
//  auto memory = m_allocator.allocate( totalBytes, pUserDataAlignment );
//  auto header = Tracking::TwnBlockHeader::create( memory, alignedUserBytes, pUserDataAlignment, pFileName, pLine, typeName );
//
//  m_tracker.add_block( header );
//  
//  return header->user_data();
//}

/**
 * deallocate
 * Given a pointer to the user region of a previously allocated block of memory, looks up the 
 * associated header, and releases the entire memory block back to the backing allocator.
 *
 * @param user_data Pointer to the user region of the allocated block to be deallocated.
 */
template <typename AllocatorT>
void DiagnosticAllocator<AllocatorT>::deallocate( void* pUserData )
{
  Tracking::TwnBlockHeader* header = Tracking::TwnBlockHeader::get_header( pUserData );
  TwnVerify( header->user_data() == pUserData );

  m_tracker.remove_block( header );

  m_allocator.deallocate( header );
}


} // namespace allok8or

