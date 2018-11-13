/**
 * @file allocator_call_helper.h
 * @brief Helper functions for calling allocator methods through the base class type.
 * 
 */
#pragma once

// Project headers
#include "allocator.h"

// Library headers


/**
 * @brief Wrapper to call through base class interface.
 */
template <typename TAlloc>
void* call_allocate(allok8or::Allocator<TAlloc>& a, size_t size, size_t alignment) {
  return a.allocate(size, alignment);
}

/**
 * @brief Wrapper to call through base class interface.
 */
template <typename TAlloc>
void* call_allocate(allok8or::Allocator<TAlloc>& a, size_t size) {
  return a.allocate(size);
}

/**
 * @brief Wrapper to call through base class interface.
 */
template <typename TAlloc>
void call_deallocate(allok8or::Allocator<TAlloc>& a, void* data) {
  a.deallocate(data);
}
