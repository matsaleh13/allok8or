/**
 * @file memory.h
 * @brief 
 * 
 */

#pragma once

namespace allok8or {

namespace memory {

void* aligned_malloc(size_t size, size_t align);
void aligned_free(void *ptr);

}

}