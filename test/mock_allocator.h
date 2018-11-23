/**
 * @file mock_allocator.h
 * @brief Simple mock allocator that derives from allok8or::Allocator.
 *
 */
#pragma once

// Project headers
#include "allocator.h"
#include "pass_through.h"

// Library headers
#include <functional>

namespace allok8or {
namespace test {

typedef std::function<void(size_t, size_t)> CbAllocate;
typedef std::function<void(void*)> CbDeallocate;

class MockAllocator : public Allocator<MockAllocator> {
public:
  explicit MockAllocator(CbAllocate on_allocate = nullptr,
                         CbDeallocate on_deallocate = nullptr)
      : m_on_allocate(on_allocate), m_on_deallocate(on_deallocate) {}

  // Public API
  void* allocate(size_t bytes,
                 size_t alignment = alignof(std::max_align_t)) const;
  void deallocate(void* user_data) const;

private:
  friend constexpr bool operator==(const MockAllocator& lhs,
                                   const MockAllocator& rhs);

  PassThroughAllocator m_allocator; // for real allocation/deallocation
  mutable CbAllocate m_on_allocate;
  mutable CbDeallocate m_on_deallocate;
};

inline void* MockAllocator::allocate(
    size_t bytes, size_t alignment /*= alignof( std::max_align_t ) */) const {
  if (m_on_allocate)
    m_on_allocate(bytes, alignment);
  return m_allocator.allocate(bytes, alignment);
}

inline void MockAllocator::deallocate(void* memory) const {
  if (m_on_deallocate)
    m_on_deallocate(memory);
  m_allocator.deallocate(memory);
}

inline constexpr bool operator==(const MockAllocator& lhs,
                                 const MockAllocator& rhs) {
  return lhs.m_allocator == rhs.m_allocator;
}

inline constexpr bool operator!=(const MockAllocator& lhs,
                                 const MockAllocator& rhs) {
  return !(lhs == rhs);
}

} // namespace test
} // namespace allok8or
