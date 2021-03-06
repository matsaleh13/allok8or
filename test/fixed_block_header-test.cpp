/**
 * @file fixed_block_header-test.cpp
 * @brief Unit tests of the FixedBlockHeader and related classes.
 *
 */

// My header
#include "fixed_block_header.h"

// Project headers
#include "align.h"

// Library headers
#include "doctest.h"
#include <string>
#include <vector>

using namespace allok8or;
using byte_t = unsigned char;

//
// BlockHeader Tests
//

/**
 * @brief Test data type for parameterizing size and alignment of allocation.
 *
 * @tparam size_n Desired size of allocation.
 * @tparam alignment_n Desired alignment of allocation.
 */
template <size_t size_n, size_t alignment_n>
struct AllocationParams {
  static const size_t size = size_n;
  static const size_t alignment = alignment_n;
};

/**
 * @brief Test fixture for type-parameterized AllocationTracker tests.
 *
 * @tparam T An instantiation of AllocationParams<size_n, alignment_n>
 */
template <typename T>
struct FixedBlockHeaderFixture {
  static const size_t user_data_size;
  static const size_t user_data_alignment;
  static const size_t aligned_user_data_size;
  static const size_t aligned_header_size;

  typedef std::vector<byte_t*> BufferList;
  BufferList m_buffers;

  byte_t* create_buffer(size_t pSize) {
    auto memory = new byte_t[pSize];
    memset(memory, 0, pSize);

    m_buffers.push_back(memory);
    return memory;
  }

  ~FixedBlockHeaderFixture() {
    for (auto buffer : m_buffers) {
      delete[] buffer;
    }
    m_buffers.clear();
  }
};

template <typename T>
const size_t FixedBlockHeaderFixture<T>::user_data_size = T::size;
template <typename T>
const size_t FixedBlockHeaderFixture<T>::user_data_alignment = T::alignment;
template <typename T>
const size_t FixedBlockHeaderFixture<T>::aligned_user_data_size =
    align::get_aligned_size(FixedBlockHeaderFixture<T>::user_data_size, FixedBlockHeaderFixture<T>::user_data_alignment);
template <typename T>
const size_t FixedBlockHeaderFixture<T>::aligned_header_size =
    align::get_aligned_size(sizeof(FixedBlockHeader<FixedBlockHeaderFixture<T>::user_data_size, FixedBlockHeaderFixture<T>::user_data_alignment>),
                            alignof(FixedBlockHeader<FixedBlockHeaderFixture<T>::user_data_size, FixedBlockHeaderFixture<T>::user_data_alignment>));

TEST_CASE_TEMPLATE_DEFINE("fixed_block_header", T, test_id) {
  using FixtureT = FixedBlockHeaderFixture<T>;
  FixtureT fixture;

  auto memory = fixture.create_buffer(FixtureT::aligned_user_data_size +
                                      FixtureT::aligned_header_size);
  void* user_data_start =
      reinterpret_cast<void*>(memory + FixtureT::aligned_header_size);

  const auto header = FixedBlockHeader<FixtureT::user_data_size, FixtureT::user_data_alignment>::create(memory);

  SUBCASE("linked_list") {
    CHECK_EQ(nullptr, header->next());
    CHECK_EQ(nullptr, header->prev());
  }

  SUBCASE("accessors") {
    CHECK_EQ(FixtureT::user_data_size, header->user_data_size);
    CHECK_EQ(FixtureT::user_data_alignment, header->user_data_alignment);
  }

  SUBCASE("addresses") {
    CHECK_EQ(reinterpret_cast<void*>(memory), reinterpret_cast<void*>(header));
    CHECK_EQ(reinterpret_cast<void*>(reinterpret_cast<byte_t*>(header) +
                                     FixtureT::aligned_header_size),
             header->user_data());
    CHECK_EQ(reinterpret_cast<void*>(memory + FixtureT::aligned_header_size),
             header->user_data());
    CHECK_EQ(user_data_start, header->user_data());
    CHECK_EQ(header, FixedBlockHeader<FixtureT::user_data_size, FixtureT::user_data_alignment>::get_header(user_data_start));
  }
}

TEST_CASE_TEMPLATE_INSTANTIATE(test_id,
                               // 4 bytes
                               AllocationParams<4, 4>,
                               AllocationParams<4, 8>,
                               AllocationParams<4, 16>,
                               AllocationParams<4, 32>,
                               // 73 bytes
                               AllocationParams<73, 4>,
                               AllocationParams<73, 8>,
                               AllocationParams<73, 16>,
                               AllocationParams<73, 32>,
                               // 1KB
                               AllocationParams<1024, 4>,
                               AllocationParams<1024, 8>,
                               AllocationParams<1024, 16>,
                               AllocationParams<1024, 32>,
                               // 100KB w/extra large alignments
                               AllocationParams<1024 * 100, 4>,
                               AllocationParams<1024 * 100, 8>,
                               AllocationParams<1024 * 100, 16>,
                               AllocationParams<1024 * 100, 32>,
                               AllocationParams<1024 * 100, 64>,
                               AllocationParams<1024 * 100, 128>,
                               // 1MB w/extra large alignments
                               AllocationParams<1024 * 1024, 4>,
                               AllocationParams<1024 * 1024, 8>,
                               AllocationParams<1024 * 1024, 16>,
                               AllocationParams<1024 * 1024, 32>,
                               AllocationParams<1024 * 1024, 64>,
                               AllocationParams<1024 * 1024, 128>,
                               // 10MB w/extra large alignments
                               AllocationParams<1024 * 1024 * 10, 4>,
                               AllocationParams<1024 * 1024 * 10, 8>,
                               AllocationParams<1024 * 1024 * 10, 16>,
                               AllocationParams<1024 * 1024 * 10, 32>,
                               AllocationParams<1024 * 1024 * 10, 64>,
                               AllocationParams<1024 * 1024 * 10, 128>);
