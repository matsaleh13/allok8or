/**
 * @file allocation_tracker-test.cpp
 * @brief Unit tests of the AllocationTracker and related classes.
 *
 */

// My header
#include "allocation_tracker.h"

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
class AllocationTrackerFixture {
public:
  const char* type_name = "class TestType<int>";

  static const size_t user_data_size = T::size;
  static const size_t user_data_alignment = T::alignment;
  static const size_t aligned_user_data_size =
      align::get_aligned_size(user_data_size, user_data_alignment);
  static const size_t aligned_header_size = align::get_aligned_size(
      sizeof(diagnostic::BlockHeader), alignof(diagnostic::BlockHeader));

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

TEST_CASE_TEMPLATE_DEFINE("block_header", T, test_id) {
  AllocationTrackerFixture<T> fixture;

  auto memory = fixture.create_buffer(fixture.aligned_user_data_size +
                                      fixture.aligned_header_size);
  void* user_data_start =
      reinterpret_cast<void*>(memory + fixture.aligned_header_size);

  auto header = diagnostic::BlockHeader::create(memory,
                                                fixture.user_data_size,
                                                fixture.user_data_alignment,
                                                fixture.type_name);

  // Linked list
  CHECK_EQ(nullptr, header->next());
  CHECK_EQ(nullptr, header->prev());

  // Accessors
  CHECK_EQ(fixture.type_name, header->type_name());
  CHECK_EQ(fixture.user_data_size, header->user_data_size());
  CHECK_EQ(fixture.user_data_alignment, header->user_data_alignment());

  // Addresses
  CHECK_EQ(reinterpret_cast<void*>(memory), reinterpret_cast<void*>(header));
  CHECK_EQ(reinterpret_cast<void*>(reinterpret_cast<byte_t*>(header) +
                                   fixture.aligned_header_size),
           header->user_data());
  CHECK_EQ(reinterpret_cast<void*>(memory + fixture.aligned_header_size),
           header->user_data());
  CHECK_EQ(user_data_start, header->user_data());
  CHECK_EQ(header, diagnostic::BlockHeader::get_header(user_data_start));
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

//
// AllocationTracker Tests
//

TEST_CASE_TEMPLATE_DEFINE("tracker", T, tracker_test_id) {
  AllocationTrackerFixture<T> fixture;

  SUBCASE("add_block") {
    diagnostic::AllocationTracker tracker;

    auto memory = fixture.create_buffer(fixture.aligned_user_data_size +
                                        fixture.aligned_header_size);
    auto header = diagnostic::BlockHeader::create(memory,
                                                  fixture.user_data_size,
                                                  fixture.user_data_alignment,
                                                  fixture.type_name);

    tracker.add_block(header);

    CHECK_EQ(nullptr, header->next());
    CHECK_EQ(nullptr, header->prev());
    CHECK_EQ(true, tracker.in_list(header));

    CHECK_EQ(header, tracker.head());
    CHECK_EQ(header, tracker.tail());
    CHECK_EQ(1, tracker.num_blocks());
    CHECK_EQ(fixture.user_data_size, tracker.num_bytes());
  }

  SUBCASE("remove_block") {
    diagnostic::AllocationTracker tracker;

    auto memory = fixture.create_buffer(fixture.aligned_user_data_size +
                                        fixture.aligned_header_size);
    auto header = diagnostic::BlockHeader::create(memory,
                                                  fixture.user_data_size,
                                                  fixture.user_data_alignment,
                                                  fixture.type_name);

    tracker.add_block(header);
    tracker.remove_block(header);

    CHECK_EQ(nullptr, header->next());
    CHECK_EQ(nullptr, header->prev());
    CHECK_EQ(false, tracker.in_list(header));

    CHECK_EQ(nullptr, tracker.head());
    CHECK_EQ(nullptr, tracker.tail());
    CHECK_EQ(0, tracker.num_blocks());
    CHECK_EQ(0, tracker.num_bytes());
  }

  SUBCASE("add_multiple_blocks") {
    diagnostic::AllocationTracker tracker;

    size_t num_blocks = 5;
    for (int ix = 0; ix < num_blocks; ++ix) {
      auto memory = fixture.create_buffer(fixture.aligned_user_data_size +
                                          fixture.aligned_header_size);
      auto header = diagnostic::BlockHeader::create(memory,
                                                    fixture.user_data_size,
                                                    fixture.user_data_alignment,
                                                    fixture.type_name);
      tracker.add_block(header);
    }

    CHECK_EQ(num_blocks, tracker.num_blocks());
    CHECK_EQ(fixture.user_data_size * num_blocks, tracker.num_bytes());

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
    diagnostic::AllocationTracker tracker;

    const size_t num_blocks = 5;
    std::vector<diagnostic::BlockHeader*> headers;
    for (int ix = 0; ix < num_blocks; ++ix) {
      auto memory = fixture.create_buffer(fixture.aligned_user_data_size +
                                          fixture.aligned_header_size);
      auto header = diagnostic::BlockHeader::create(memory,
                                                    fixture.user_data_size,
                                                    fixture.user_data_alignment,
                                                    fixture.type_name);
      tracker.add_block(header);

      headers.push_back(header);
    }

    // Remove a block from the middle.
    auto current_num_blocks = num_blocks - 1;
    const auto header_to_remove = headers[current_num_blocks / 2];
    tracker.remove_block(header_to_remove);

    CHECK_EQ(nullptr, header_to_remove->next());
    CHECK_EQ(nullptr, header_to_remove->prev());
    CHECK_EQ(false, tracker.in_list(header_to_remove));

    CHECK_EQ(current_num_blocks, tracker.num_blocks());
    CHECK_EQ(fixture.user_data_size * current_num_blocks, tracker.num_bytes());

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
    diagnostic::AllocationTracker tracker;

    size_t num_blocks = 5;
    std::vector<diagnostic::BlockHeader*> headers;
    for (int ix = 0; ix < num_blocks; ++ix) {
      auto memory = fixture.create_buffer(fixture.aligned_user_data_size +
                                          fixture.aligned_header_size);
      auto header = diagnostic::BlockHeader::create(memory,
                                                    fixture.user_data_size,
                                                    fixture.user_data_alignment,
                                                    fixture.type_name);
      tracker.add_block(header);

      headers.push_back(header); // Reverse order to the tracker list.
    }

    // Remove first block (last one added)
    auto current_num_blocks = num_blocks - 1;
    auto header_to_remove = headers[headers.size() - 1]; 
    tracker.remove_block(header_to_remove);

    CHECK_EQ(nullptr, header_to_remove->next());
    CHECK_EQ(nullptr, header_to_remove->prev());
    CHECK_EQ(false, tracker.in_list(header_to_remove));

    CHECK_EQ(current_num_blocks, tracker.num_blocks());
    CHECK_EQ(fixture.user_data_size * current_num_blocks, tracker.num_bytes());

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
    diagnostic::AllocationTracker tracker;

    size_t num_blocks = 5;
    std::vector<diagnostic::BlockHeader*> headers;
    for (int ix = 0; ix < num_blocks; ++ix) {
      auto memory = fixture.create_buffer(fixture.aligned_user_data_size +
                                          fixture.aligned_header_size);
      auto header = diagnostic::BlockHeader::create(memory,
                                                    fixture.user_data_size,
                                                    fixture.user_data_alignment,
                                                    fixture.type_name);
      tracker.add_block(header);

      headers.push_back(header); // Reverse order to the tracker list.
    }

    // Remove last block (first one added)
    auto current_num_blocks = num_blocks - 1;
    auto header_to_remove = headers[0]; 
    tracker.remove_block(header_to_remove);

    CHECK_EQ(nullptr, header_to_remove->next());
    CHECK_EQ(nullptr, header_to_remove->prev());
    CHECK_EQ(false, tracker.in_list(header_to_remove));

    CHECK_EQ(current_num_blocks, tracker.num_blocks());
    CHECK_EQ(fixture.user_data_size * current_num_blocks, tracker.num_bytes());

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
                               // Tiny user data
                               AllocationParams<4, 4>,
                               AllocationParams<4, 8>,
                               AllocationParams<4, 16>,
                               AllocationParams<4, 32>,
                               // Small user data
                               AllocationParams<72, 4>,
                               AllocationParams<72, 8>,
                               AllocationParams<72, 16>,
                               AllocationParams<72, 32>,
                               // 1K user data
                               AllocationParams<1024, 4>,
                               AllocationParams<1024, 8>,
                               AllocationParams<1024, 16>,
                               AllocationParams<1024, 32>);
