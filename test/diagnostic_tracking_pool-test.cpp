/**
 * @file diagnostic_allocation_tracker-test.cpp
 * @brief Unit tests of the AllocationTracker and related classes.
 *
 */

// My header
#include "diagnostic/tracking_pool.h"
#include "diagnostic/block_header.h"

// Project headers
#include "align.h"

// Library headers
#include "doctest.h"
#include <memory>
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
struct AllocationTrackerFixture {
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

  ~AllocationTrackerFixture() {
    for (auto buffer : m_buffers) {
      delete[] buffer;
    }
    m_buffers.clear();
  }
};

template <typename T>
const size_t AllocationTrackerFixture<T>::user_data_size = T::size;
template <typename T>
const size_t AllocationTrackerFixture<T>::user_data_alignment = T::alignment;
template <typename T>
const size_t AllocationTrackerFixture<T>::aligned_user_data_size =
    align::get_aligned_size(user_data_size, user_data_alignment);
template <typename T>
const size_t AllocationTrackerFixture<T>::aligned_header_size =
    align::get_aligned_size(sizeof(diagnostic::BlockHeader),
                            alignof(diagnostic::BlockHeader));


//
// AllocationTracker Tests
//

TEST_CASE_TEMPLATE_DEFINE("pool", T, tracker_test_id) {
  using FixtureT = AllocationTrackerFixture<T>;
  FixtureT fixture;

  SUBCASE("add_block") {
    diagnostic::AllocationTrackingPool pool;

    auto memory = fixture.create_buffer(FixtureT::aligned_user_data_size +
                                        FixtureT::aligned_header_size);
    auto header = diagnostic::BlockHeader::create(memory,
                                                  FixtureT::user_data_size,
                                                  FixtureT::user_data_alignment);

    pool.add(header);

    CHECK_EQ(nullptr, header->next());
    CHECK_EQ(nullptr, header->prev());
    CHECK_EQ(true, pool.in_list(header));

    CHECK_EQ(header, pool.head());
    CHECK_EQ(header, pool.tail());
    CHECK_EQ(1, pool.num_blocks());
    CHECK_EQ(FixtureT::user_data_size, pool.num_bytes());
  }

  SUBCASE("remove_block") {
    diagnostic::AllocationTrackingPool tracker;

    auto memory = fixture.create_buffer(FixtureT::aligned_user_data_size +
                                        FixtureT::aligned_header_size);
    auto header = diagnostic::BlockHeader::create(memory,
                                                  FixtureT::user_data_size,
                                                  FixtureT::user_data_alignment);

    tracker.add(header);
    tracker.remove(header);

    CHECK_EQ(nullptr, header->next());
    CHECK_EQ(nullptr, header->prev());
    CHECK_EQ(false, tracker.in_list(header));

    CHECK_EQ(nullptr, tracker.head());
    CHECK_EQ(nullptr, tracker.tail());
    CHECK_EQ(0, tracker.num_blocks());
    CHECK_EQ(0, tracker.num_bytes());
  }

  SUBCASE("add_several_blocks") {
    diagnostic::AllocationTrackingPool tracker;

    size_t num_blocks = 5;
    for (int ix = 0; ix < num_blocks; ++ix) {
      auto memory = fixture.create_buffer(FixtureT::aligned_user_data_size +
                                          FixtureT::aligned_header_size);
      auto header =
          diagnostic::BlockHeader::create(memory,
                                          FixtureT::user_data_size,
                                          FixtureT::user_data_alignment);
      tracker.add(header);
    }

    CHECK_EQ(num_blocks, tracker.num_blocks());
    CHECK_EQ(FixtureT::user_data_size * num_blocks, tracker.num_bytes());

    // Iterate to the last block.
    int count = 0;
    auto block = tracker.head();
    while (++count < num_blocks) {
      block = block->next();
    }

    // Last block points to nothing.
    CHECK_EQ(nullptr, block->next());

    // Iterate back to the first block.
    count = 0;
    block = tracker.tail();
    while (++count < num_blocks) {
      block = block->prev();
    }

    // First block is head.
    CHECK_EQ(tracker.head(), block);

    // Head block has nothing before it.
    CHECK_EQ(nullptr, block->prev());
  }

  SUBCASE("remove_middle_block") {
    diagnostic::AllocationTrackingPool tracker;

    const size_t num_blocks = 5;
    std::vector<diagnostic::BlockHeader*> headers;
    for (int ix = 0; ix < num_blocks; ++ix) {
      auto memory = fixture.create_buffer(FixtureT::aligned_user_data_size +
                                          FixtureT::aligned_header_size);
      auto header =
          diagnostic::BlockHeader::create(memory,
                                          FixtureT::user_data_size,
                                          FixtureT::user_data_alignment);
      tracker.add(header);

      headers.push_back(header);
    }

    // Remove a block from the middle.
    auto current_num_blocks = num_blocks - 1;
    const auto header_to_remove = headers[current_num_blocks / 2];
    tracker.remove(header_to_remove);

    CHECK_EQ(nullptr, header_to_remove->next());
    CHECK_EQ(nullptr, header_to_remove->prev());
    CHECK_EQ(false, tracker.in_list(header_to_remove));

    CHECK_EQ(current_num_blocks, tracker.num_blocks());
    CHECK_EQ(FixtureT::user_data_size * current_num_blocks,
             tracker.num_bytes());

    // Iterate to the last block.
    int count = 0;
    auto block = tracker.head();
    while (++count < current_num_blocks) {
      block = block->next();
    }

    // Last block points to nothing.
    CHECK_EQ(nullptr, block->next());

    // Iterate back to the first block.
    count = 0;
    block = tracker.tail();
    while (++count < current_num_blocks) {
      block = block->prev();
    }

    // First block is head.
    CHECK_EQ(tracker.head(), block);

    // Head block has nothing before it.
    CHECK_EQ(nullptr, block->prev());
  }

  SUBCASE("remove_first_block") {
    diagnostic::AllocationTrackingPool tracker;

    size_t num_blocks = 5;
    std::vector<diagnostic::BlockHeader*> headers;
    for (int ix = 0; ix < num_blocks; ++ix) {
      auto memory = fixture.create_buffer(FixtureT::aligned_user_data_size +
                                          FixtureT::aligned_header_size);
      auto header =
          diagnostic::BlockHeader::create(memory,
                                          FixtureT::user_data_size,
                                          FixtureT::user_data_alignment);
      tracker.add(header);

      headers.push_back(header); // Reverse order to the pool list.
    }

    // Remove first block (last one added)
    auto current_num_blocks = num_blocks - 1;
    auto header_to_remove = headers[headers.size() - 1];
    tracker.remove(header_to_remove);

    CHECK_EQ(nullptr, header_to_remove->next());
    CHECK_EQ(nullptr, header_to_remove->prev());
    CHECK_EQ(false, tracker.in_list(header_to_remove));

    CHECK_EQ(current_num_blocks, tracker.num_blocks());
    CHECK_EQ(FixtureT::user_data_size * current_num_blocks,
             tracker.num_bytes());

    // Iterate to the last block.
    int count = 0;
    auto block = tracker.head();
    while (++count < current_num_blocks) {
      block = block->next();
    }

    // Last block points to nothing.
    CHECK_EQ(nullptr, block->next());

    // Iterate back to the first block.
    count = 0;
    block = tracker.tail();
    while (++count < current_num_blocks) {
      block = block->prev();
    }

    // First block is head.
    CHECK_EQ(tracker.head(), block);

    // Head block has nothing before it.
    CHECK_EQ(nullptr, block->prev());
  }

  SUBCASE("remove_last_block") {
    diagnostic::AllocationTrackingPool tracker;

    size_t num_blocks = 5;
    std::vector<diagnostic::BlockHeader*> headers;
    for (int ix = 0; ix < num_blocks; ++ix) {
      auto memory = fixture.create_buffer(FixtureT::aligned_user_data_size +
                                          FixtureT::aligned_header_size);
      auto header =
          diagnostic::BlockHeader::create(memory,
                                          FixtureT::user_data_size,
                                          FixtureT::user_data_alignment);
      tracker.add(header);

      headers.push_back(header); // Reverse order to the pool list.
    }

    // Remove last block (first one added)
    auto current_num_blocks = num_blocks - 1;
    auto header_to_remove = headers[0];
    tracker.remove(header_to_remove);

    CHECK_EQ(nullptr, header_to_remove->next());
    CHECK_EQ(nullptr, header_to_remove->prev());
    CHECK_EQ(false, tracker.in_list(header_to_remove));

    CHECK_EQ(current_num_blocks, tracker.num_blocks());
    CHECK_EQ(FixtureT::user_data_size * current_num_blocks,
             tracker.num_bytes());

    // Iterate to the last block.
    int count = 0;
    auto block = tracker.head();
    while (++count < current_num_blocks) {
      block = block->next();
    }

    // Last block points to nothing.
    CHECK_EQ(nullptr, block->next());

    // Iterate back to the first block.
    count = 0;
    block = tracker.tail();
    while (++count < current_num_blocks) {
      block = block->prev();
    }

    // First block is head.
    CHECK_EQ(tracker.head(), block);

    // Head block has nothing before it.
    CHECK_EQ(nullptr, block->prev());
  }
}

TEST_CASE_TEMPLATE_INSTANTIATE(tracker_test_id,
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
                               // 1K bytes
                               AllocationParams<1024, 4>,
                               AllocationParams<1024, 8>,
                               AllocationParams<1024, 16>,
                               AllocationParams<1024, 32>);
