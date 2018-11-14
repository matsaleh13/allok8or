/**
 * @file global.h
 * @brief Header for providing global access to a single allocator.
 *
 */
#pragma once

// Project headers
#include "allocator.h"

// Library headers

namespace allok8or {
namespace global {

/**
 * @brief Non-singleton implementation of global instance.
 *
 * TODO: Thread safety.
 *
 * NOTE: Because sometimes you really *do* need a global!
 *
 * @tparam TAllocator Type of the allocator implementation class.
 */
template <typename TAllocator>
class GlobalAllocator {
  GlobalAllocator() = delete;
  ~GlobalAllocator() = delete;

public:
  constexpr static void init(Allocator<TAllocator>* allocator);
  constexpr static void cleanup();

  constexpr static void* allocate(size_t size);
  constexpr static void* allocate(size_t size, size_t alignment);
  constexpr static void deallocate(void* data);

  constexpr static const TAllocator* get() { return m_allocator; }
private:
  static TAllocator* m_allocator;
};

/**
 * @brief Static init of the instance.
 * 
 * @tparam TAllocator Type of the allocator implementation class.
 */
template <typename TAllocator>
TAllocator* GlobalAllocator<TAllocator>::m_allocator = nullptr;


/**
 * @brief Initialize the static instance of the global allocator.
 *
 * NOTE: Currently caller must manage the memory.
 * TODO: Thread safety.
 *
 * @tparam TAllocator Implementation type passed to Allocator<typename T>
 * @param allocator Pointer to the allocator instance to be used globally.
 */
template <typename TAllocator>
inline constexpr void GlobalAllocator<TAllocator>::init(Allocator<TAllocator>* allocator) {
  m_allocator = static_cast<TAllocator*>(allocator);
}

/**
 * @brief Clear the static instance pointer.
 *
 * @tparam TAllocator Implementation type passed to Allocator<typename T>
 */
template <typename TAllocator>
inline constexpr void GlobalAllocator<TAllocator>::cleanup() {
  m_allocator = nullptr;
}

/**
 * @brief Allocate a block of memory using the global allocator.
 * 
 * TODO: Handle OOM (i.e. throw/noexcept)
 * 
 * @tparam TAllocator Implementation type passed to Allocator<typename T>
 * @param size The number of bytes to allocate.
 * @return void* Pointer to the allocated block of memory.
 */
template <typename TAllocator>
inline constexpr void* GlobalAllocator<TAllocator>::allocate(size_t size) {
  return m_allocator->allocate(size);
}

/**
 * @brief Allocate a block of memory using the global allocator.
 * 
 * TODO: Handle OOM (i.e. throw/noexcept)
 * 
 * @tparam TAllocator Implementation type passed to Allocator<typename T>
 * @param size The number of bytes to allocate.
 * @param alignment Alignment to use for the memory block.
 * @return void* Pointer to the allocated block of memory.
 */
template <typename TAllocator>
inline constexpr void* GlobalAllocator<TAllocator>::allocate(size_t size, size_t alignment) {
  return m_allocator->allocate(size, alignment);
}

/**
 * @brief Deallocate the memory via the given pointer using the global allocator.
 * 
 * NOTE: Deallocated memory *must* have been allocated by the same allocator.
 * TODO: Handle OOM (i.e. throw/noexcept)
 * TODO: Validate input.
 * 
 * @tparam TAllocator Implementation type passed to Allocator<typename T>
 * @param data Pointer to the memory block to deallocate.
 */
template <typename TAllocator>
inline constexpr void GlobalAllocator<TAllocator>::deallocate(void* data) {
  return m_allocator->deallocate(data);
}

} // namespace global
} // namespace allok8or
