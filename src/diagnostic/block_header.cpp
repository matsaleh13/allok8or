/**
 * @file diagnostic_block_header.cpp
 * @brief Memory block header for tracking allocations.
 *
 * @copyright Copyright (c) 2018
 *
 */

// My header
#include "block_header.h"

// Project headers
#include "../align.h"

// Library headers

namespace allok8or {
namespace diagnostic {
//
// BlockSignature implementation.
//
const BlockSignature BlockHeader::BLOCK_SIGNATURE;

//
// BlockHeader Implementation
//

/**
 * @brief Factory method for ensuring ctor params are valid.
 *
 * @param block_start Start address of the complete memory block (and pointer to
 * the BlockHeader).
 * @param user_data_size Size of the user portion of the memory block.
 * @param user_data_alignment Alignment of the data in the user portion of the
 * memory block.
 * @param type_name String representation of the data type of the user memory.
 * @param file_name File name of the caller details.
 * @param line Line number of the caller details.
 * @return BlockHeader* Pointer to the full memory block.
 */
BlockHeader* BlockHeader::create(void* block_start,
                                 size_t user_data_size,
                                 size_t user_data_alignment,
                                 const char* type_name /*= nullptr*/,
                                 const char* file_name /*= nullptr*/,
                                 int line /*= 0 */) {
  assert(block_start);
  assert(user_data_size);
  assert(user_data_alignment);

  if (!block_start || !user_data_size || !user_data_alignment)
    return nullptr;

  // Use placement new to init the header in the given memory block.
  BlockHeader* header = new (block_start) BlockHeader(
      user_data_size, user_data_alignment, file_name, line, type_name);

  return header;
}

} // namespace diagnostic
} // namespace allok8or
