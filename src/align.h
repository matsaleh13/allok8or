#pragma once 

#include <stddef.h>

namespace allok8or
{
namespace align
  {

/**
 * @brief Get the first address aligned to alignment that is lower than the given address.
 * 
 * @param address Starting address.
 * @param alignment Alignment to use to get the desired address.
 * @return Pointer to the desired address (aligned). 
 */
inline
void* get_prev_aligned_address( void* address, size_t alignment )
{
  return (void*) ( static_cast<size_t>( ~( alignment - 1 ) ) & ( reinterpret_cast<size_t>( address ) ) );
}

/**
 * @brief Get the first address aligned to alignment that is higher than the given address.
 * 
 * @param address Starting address.
 * @param alignment Alignment to use to get the desired address.
 * @return Pointer to the desired address (aligned). 
 */
inline
void* get_next_aligned_address( void* address, size_t alignment )
{
  return (void*) ( static_cast<size_t>( ~( alignment - 1 ) ) & ( reinterpret_cast<size_t>( address ) + static_cast<size_t>( alignment - 1 ) ) );
}

/**
* Return the number of bytes required to contain data of the given size using the given alignment.
*/

/**
 * @brief Get the number of bytes needed to contain data of the given size using the given alignment.
 * 
 * @param size Size of the data to be contained.
 * @param alignment Alignment to use to get the desired size.
 * @return Aligned size required to contain the data.
 */
inline
constexpr size_t get_aligned_size(size_t size, size_t alignment)
{
  return alignment ? (size + alignment - ( size % alignment ) % alignment) : size;
}





} // namespace align
} // namespace allok8or
