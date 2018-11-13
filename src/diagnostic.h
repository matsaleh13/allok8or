/**
 * @file diagnostic.h
 * @brief Header file for the DiagnosticAllocator, which tracks and reports on
 * allocations.
 *
 */
#pragma once

// Project headers
#include "allocator.h"
#include "diagnostic_allocation_tracker.h"
#include "diagnostic_header.h"

// Library headers
#include <cstddef>

namespace allok8or {

/**
 * DiagnosticAllocator
 *
 * An allocator that tracks net allocations for the purpose of identifying
 * memory leaks.
 *
 * This allocator does not itself manage allocated memory. Rather, another
 * allocator passed in as a ctor argument does that.
 */
template <typename TAllocator>
class DiagnosticAllocator : public Allocator<DiagnosticAllocator<TAllocator>> {
public:
  DiagnosticAllocator(Allocator<TAllocator>& allocator);
  ~DiagnosticAllocator(){};

  void* allocate(size_t size);
  void* allocate(size_t size, size_t alignment);
  void deallocate(void* user_data);

  const diagnostic::AllocationTracker& Tracker() const { return m_tracker; }

private:
  Allocator<TAllocator>& m_allocator;
  diagnostic::AllocationTracker m_tracker;
};

/**
 * @brief Constructor
 * 
 * @tparam TAllocator Type of the backing allocator. 
 * @param allocator The backing allocator to actually allocate/deallocate heap
 */
template <typename TAllocator>
DiagnosticAllocator<TAllocator>::DiagnosticAllocator(Allocator<TAllocator>& allocator)
    : m_allocator(allocator) {}

/**
 * @brief Gets memory from the backing allocator and returns a pointer to the
 * user portion.
 *
 * NOTE: Allocates enough extra memory to hold the data used for tracking and
 * detecting leaks. The pointer returned to the caller is the starting address
 * of the user portion of the memory block.
 *
 * @tparam TAllocator Type of the backing allocator.
 * @param user_data_size The size of the memory requested by the caller.
 * @param user_data_alignment The alignment requested by the caller.
 * @return void* Pointer to the user portion of the memory.
 */
template <typename TAllocator>
void* allok8or::DiagnosticAllocator<TAllocator>::allocate(
    size_t user_data_size, size_t user_data_alignment) {
  auto aligned_user_data_size =
      align::get_aligned_size(user_data_size, user_data_alignment);
  auto total_size = aligned_user_data_size +
                    align::get_aligned_size(sizeof(diagnostic::BlockHeader),
                                            alignof(diagnostic::BlockHeader));

  auto memory_block = m_allocator.allocate(total_size, user_data_alignment);
  auto header = diagnostic::BlockHeader::create(
      memory_block, aligned_user_data_size, user_data_alignment);

  m_tracker.add(header);

  return header->user_data();
}

/**
 * Given a pointer to the user region of a previously allocated block of memory,
 * looks up the associated header, and releases the entire memory block back to
 * the backing allocator.
 *
 * @param user_data Pointer to the user region of the allocated block to be
 * deallocated.
 */

/**
 * @brief Releases memory back to the backing allocator.
 *
 * NOTE: Looks up the header using the pointer to the user memory provided.
 *
 * @tparam TAllocator Type of the backing allocator.
 * @param user_data Pointer to the user portion of the memory block to
 * deallocate.
 */
template <typename TAllocator>
void DiagnosticAllocator<TAllocator>::deallocate(void* user_data) {
  diagnostic::BlockHeader* header =
      diagnostic::BlockHeader::get_header(user_data);
  assert(header->user_data() == user_data);

  m_tracker.remove(header);
  m_allocator.deallocate(header);
}

} // namespace allok8or
