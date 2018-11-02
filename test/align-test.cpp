/**
 * @file align-test.cpp
 * @brief Unit tests of functions in the align namespace.
 *
 */

#include "align.h"
#include "doctest.h"

#include <memory>

struct TestData {
  size_t alignment;
  uintptr_t address;
  uintptr_t expected;
};

// Data
const TestData prev_data[3] = {
    TestData{2, 0x0000000000000003, 0x0000000000000002},
    TestData{8, 0x0000000000000009, 0x0000000000000008},
    TestData{16, 0x0000000000000011, 0x0000000000000010}};

void test_get_prev_aligned_address(const TestData& data) {
  size_t alignment = data.alignment;
  uintptr_t address = data.address;
  uintptr_t expected = data.expected;
  CHECK(expected ==
        reinterpret_cast<uintptr_t>(allok8or::align::get_prev_aligned_address(
            (void*)address, alignment)));
}

TEST_CASE("get_prev_aligned_address") {
  for (auto d : prev_data) {
    CAPTURE(d);
    test_get_prev_aligned_address(d);
  }
}

const TestData next_data[3] = {
    TestData{2, 0x0000000000000003, 0x0000000000000004},
    TestData{8, 0x0000000000000009, 0x0000000000000010},
    TestData{16, 0x0000000000000011, 0x0000000000000020}};

void test_get_next_aligned_address(const TestData& data) {
  size_t alignment = data.alignment;
  uintptr_t address = data.address;
  uintptr_t expected = data.expected;
  CHECK(expected ==
        reinterpret_cast<uintptr_t>(allok8or::align::get_next_aligned_address(
            (void*)address, alignment)));
}

TEST_CASE("get_next_aligned_address") {
  for (auto d : next_data) {
    CAPTURE(d);
    test_get_next_aligned_address(d);
  }
}

struct TestSizeData {
  size_t alignment;
  size_t size;
  size_t expected;
};

const TestSizeData size_data[12] = {
    TestSizeData{2, 17, 18},  TestSizeData{4, 17, 20},
    TestSizeData{8, 17, 24},  TestSizeData{16, 17, 32},
    TestSizeData{32, 17, 32}, TestSizeData{2, 31, 32},
    TestSizeData{4, 31, 32},  TestSizeData{8, 31, 32},
    TestSizeData{16, 31, 32}, TestSizeData{32, 31, 32},
    TestSizeData{0, 31, 31},  TestSizeData{2, 0, 2}};

void test_get_aligned_size(const TestSizeData& data) {
  size_t alignment = data.alignment;
  size_t size = data.size;
  size_t expected = data.expected;
  CHECK(expected == allok8or::align::get_aligned_size(size, alignment));
}

TEST_CASE("get_aligned_size") {
  for (auto d: size_data) {
    test_get_aligned_size(d);
  }
}
