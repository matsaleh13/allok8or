/**
 * @file memory.h
 * @brief Cross-platform memory-related functions.
 * 
 */

#pragma once

#include <stddef.h>

namespace allok8or {

namespace memory {

void* aligned_malloc(size_t size, size_t align);
void aligned_free(void *ptr);

}

}