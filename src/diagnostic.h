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
#include "diagnostic_block_header.h"

// Library headers
#include <cstddef>

namespace allok8or {

/**
 * @brief An allocator that tracks net allocations for the purpose of
 * identifying memory leaks.
 *
 * This allocator does not itself manage allocated memory. Rather, another
 * allocator passed in as a ctor argument does that.
 *
 * NOTE: This allocator cannot be copied; it must be shared.
 * TODO: Thread safety.
 *
 * @tparam TBackingAllocator Type of the backing allocator; must be an
 * allok8or::Allocator-derived class.
 */
template <typename TBackingAllocator>
class DiagnosticAllocator
    : public Allocator<DiagnosticAllocator<TBackingAllocator>> {
public:
  DiagnosticAllocator(Allocator<TBackingAllocator>& allocator);
  ~DiagnosticAllocator(){};

  // No copies; share this when appropriate.
  DiagnosticAllocator(const DiagnosticAllocator&) = delete;
  DiagnosticAllocator& operator=(const DiagnosticAllocator&) = delete;

  void* allocate(size_t size) const;
  void* allocate(size_t size, size_t alignment) const;
  void deallocate(void* user_data) const;

  const diagnostic::AllocationTracker& Tracker() const { return m_tracker; }

private:
  TBackingAllocator& m_allocator;
  mutable diagnostic::AllocationTracker
      m_tracker; // mutable required because all Allocator<T>-derived classes
                 // must have const API.
};

template <typename TBackingAllocator>
constexpr bool operator==(const DiagnosticAllocator<TBackingAllocator>& lhs,
                          const DiagnosticAllocator<TBackingAllocator>& rhs) {
  return &lhs == &rhs;
}

template <typename TBackingAllocator>
constexpr bool operator!=(const DiagnosticAllocator<TBackingAllocator>& lhs,
                          const DiagnosticAllocator<TBackingAllocator>& rhs) {
  return !(&lhs == &rhs);
}

/**
 * @brief Constructor
 *
 * @tparam TBackingAllocator Type of the backing allocator.
 * @param allocator The backing allocator to actually allocate/deallocate heap
 */
template <typename TBackingAllocator>
DiagnosticAllocator<TBackingAllocator>::DiagnosticAllocator(
    Allocator<TBackingAllocator>& allocator)
    : m_allocator(static_cast<TBackingAllocator&>(allocator)) {}

/**
 * @brief Gets memory from the backing allocator using default alignment and
 * returns a pointer to the user portion.
 *
 * NOTE: Allocates enough extra memory to hold the data used for tracking and
 * detecting leaks. The pointer returned to the caller is the starting address
 * of the user portion of the memory block.
 *
 * @tparam TBackingAllocator Type of the backing allocator.
 * @param user_data_size The size of the memory requested by the caller.
 * @return void* Pointer to the user portion of the memory.
 */
template <typename TBackingAllocator>
void* allok8or::DiagnosticAllocator<TBackingAllocator>::allocate(
    size_t user_data_size) const {
  return allocate(user_data_size, alignof(std::max_align_t));
}

/**
 * @brief Gets memory from the backing allocator and returns a pointer to the
 * user portion.
 *
 * NOTE: Allocates enough extra memory to hold the data used for tracking and
 * detecting leaks. The pointer returned to the caller is the starting address
 * of the user portion of the memory block.
 *
 * @tparam TBackingAllocator Type of the backing allocator.
 * @param user_data_size The size of the memory requested by the caller.
 * @param user_data_alignment The alignment requested by the caller.
 * @return void* Pointer to the user portion of the memory.
 */
template <typename TBackingAllocator>
void* allok8or::DiagnosticAllocator<TBackingAllocator>::allocate(
    size_t user_data_size, size_t user_data_alignment) const {
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
 * @brief Releases memory back to the backing allocator.
 *
 * NOTE: Looks up the header using the pointer to the user memory provided.
 *
 * @tparam TBackingAllocator Type of the backing allocator.
 * @param user_data Pointer to the user portion of the memory block to
 * deallocate.
 */
template <typename TBackingAllocator>
void DiagnosticAllocator<TBackingAllocator>::deallocate(void* user_data) const {
  diagnostic::BlockHeader* header =
      diagnostic::BlockHeader::get_header(user_data);
  assert(header->user_data() == user_data);

  m_tracker.remove(header);
  m_allocator.deallocate(header);
}

} // namespace allok8or
