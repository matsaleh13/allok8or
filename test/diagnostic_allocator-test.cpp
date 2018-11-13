/**
 * @file diagnostic_allocator-test.cpp
 * @brief Unit tests of the DiagnosticAllocator class.
 *
 */

// My header
#include "diagnostic.h"

// Project headers
#include "allocator_call_helper.h"
#include "allocator.h"
#include "align.h"
#include "pass_through.h"

// Library headers
#include "doctest.h"
#include <array>
#include <memory>


using namespace allok8or;

//
// Tests
//

// Test types to allocate
template <typename T>
class FooT {
public:
  FooT(const std::string& name, const T& data) : m_data(data), m_name(name) {}
  T m_data;
  std::string m_name;
};

template <typename T, int N>
class FooNT {
public:
  FooNT() : m_data() {}
  std::array<T, N> m_data;
};

template <typename T>
class BarT : FooNT<BarT<T>, 3> {
public:
  BarT(const std::string& name) : m_name(name) {}
  std::string m_name;
};

TEST_CASE("create") {
  PassThroughAllocator pass_through;
  DiagnosticAllocator<PassThroughAllocator> allocator(pass_through);

  CHECK_EQ(0, allocator.Tracker().num_blocks());
  CHECK_EQ(0, allocator.Tracker().num_bytes());
}

TEST_CASE("allocate") {
  PassThroughAllocator pass_through;
  DiagnosticAllocator<PassThroughAllocator> allocator(pass_through);

  using FooInt = FooT<int>;

  auto memory_block = call_allocate(allocator, sizeof(FooInt), alignof(FooInt));

  // placement new to simulate real code.
  auto foo = new (memory_block) FooInt("Foo42", 42);

  CHECK_EQ(1, allocator.Tracker().num_blocks());
  CHECK_EQ(align::get_aligned_size(sizeof(FooInt), alignof(FooInt)),
           allocator.Tracker().num_bytes());

  CHECK_EQ("Foo42", foo->m_name);
  CHECK_EQ(42, foo->m_data);
}

TEST_CASE("allocate_several") {
  PassThroughAllocator pass_through;
  DiagnosticAllocator<PassThroughAllocator> allocator(pass_through);

  using FooInt = FooT<int>;
  using FooN42 = FooNT<std::string, 42>;

  auto memory1 = call_allocate(allocator, sizeof(FooInt), alignof(FooInt));
  auto foo42 = new (memory1) FooInt("Foo42", 42);

  auto memory2 = call_allocate(allocator, sizeof(FooN42), alignof(FooN42));
  auto fooN42 = new (memory2) FooN42();

  auto memory3 = call_allocate(allocator, sizeof(FooInt), alignof(FooInt));
  auto foo42_3 = new (memory3) FooInt("Foo42_3", 423);

  auto memory4 = call_allocate(allocator, sizeof(FooN42), alignof(FooN42));
  auto fooN42_4 = new (memory4) FooN42();

  CHECK_EQ(4, allocator.Tracker().num_blocks());

  // Four instances of two different types.
  int64_t size = align::get_aligned_size(sizeof(FooInt), alignof(FooInt)) * 2 +
                 align::get_aligned_size(sizeof(FooN42), alignof(FooN42)) * 2;
  CHECK_EQ(size, allocator.Tracker().num_bytes());

  CHECK_EQ("Foo42", foo42->m_name);
  CHECK_EQ(42, foo42->m_data);

  CHECK_EQ(42, fooN42->m_data.size());

  CHECK_EQ("Foo42_3", foo42_3->m_name);
  CHECK_EQ(423, foo42_3->m_data);

  CHECK_EQ(42, fooN42_4->m_data.size());
}

TEST_CASE("deallocate") {
  PassThroughAllocator pass_through;
  DiagnosticAllocator<PassThroughAllocator> allocator(pass_through);

  using FooInt = FooT<int>;

  auto memory_block = call_allocate(allocator, sizeof(FooInt), alignof(FooInt));

  // placement new to simulate real code.
  auto foo = new (memory_block) FooInt("Foo42", 42);
  foo->~FooT();

  call_deallocate(allocator, memory_block);

  CHECK_EQ(0, allocator.Tracker().num_blocks());
  CHECK_EQ(0, allocator.Tracker().num_bytes());
}

TEST_CASE("deallocate_several") {
  PassThroughAllocator pass_through;
  DiagnosticAllocator<PassThroughAllocator> allocator(pass_through);

  using FooInt = FooT<int>;
  using FooN42 = FooNT<std::string, 42>;

  auto memory1 = call_allocate(allocator, sizeof(FooInt), alignof(FooInt));
  auto foo42 = new (memory1) FooInt("Foo42", 42);

  auto memory2 = call_allocate(allocator, sizeof(FooN42), alignof(FooN42));
  auto fooN42 = new (memory2) FooN42();

  auto memory3 = call_allocate(allocator, sizeof(FooInt), alignof(FooInt));
  auto foo42_3 = new (memory3) FooInt("Foo42_3", 423);

  auto memory4 = call_allocate(allocator, sizeof(FooN42), alignof(FooN42));
  auto fooN42_4 = new (memory4) FooN42();

  foo42->~FooT();
  call_deallocate(allocator, memory1);

  fooN42->~FooN42();
  call_deallocate(allocator, memory2);

  foo42_3->~FooT();
  call_deallocate(allocator, memory3);

  fooN42_4->~FooN42();
  call_deallocate(allocator, memory4);

  CHECK_EQ(0, allocator.Tracker().num_blocks());
  CHECK_EQ(0, allocator.Tracker().num_bytes());
}

TEST_CASE("deallocate_one_of_several") {
  PassThroughAllocator pass_through;
  DiagnosticAllocator<PassThroughAllocator> allocator(pass_through);

  using FooInt = FooT<int>;
  using FooN42 = FooNT<std::string, 42>;

  auto memory1 = call_allocate(allocator, sizeof(FooInt), alignof(FooInt));
  auto foo42 = new (memory1) FooInt("Foo42", 42);

  auto memory2 = call_allocate(allocator, sizeof(FooN42), alignof(FooN42));
  auto fooN42 = new (memory2) FooN42();

  auto memory3 = call_allocate(allocator, sizeof(FooInt), alignof(FooInt));
  auto foo42_3 = new (memory3) FooInt("Foo42_3", 423);

  auto memory4 = call_allocate(allocator, sizeof(FooN42), alignof(FooN42));
  auto fooN42_4 = new (memory4) FooN42();

  // only one out of 4
  fooN42->~FooN42();
  call_deallocate(allocator, memory2);

  CHECK_EQ(3, allocator.Tracker().num_blocks());
  // 3 instances left - two of one type
  int64_t size = align::get_aligned_size(sizeof(FooInt), alignof(FooInt)) * 2 +
                 align::get_aligned_size(sizeof(FooN42), alignof(FooN42));
  CHECK_EQ(size, allocator.Tracker().num_bytes());

  CHECK_EQ("Foo42", foo42->m_name);
  CHECK_EQ(42, foo42->m_data);

  CHECK_EQ("Foo42_3", foo42_3->m_name);
  CHECK_EQ(423, foo42_3->m_data);

  CHECK_EQ(42, fooN42_4->m_data.size());
}

TEST_CASE("allocate_after_deallocate") {
  PassThroughAllocator pass_through;
  DiagnosticAllocator<PassThroughAllocator> allocator(pass_through);

  using FooInt = FooT<int>;
  using FooN42 = FooNT<std::string, 42>;

  auto memory1 = call_allocate(allocator, sizeof(FooInt), alignof(FooInt));

  // placement new to simulate real code.
  auto foo = new (memory1) FooInt("Foo42", 42);
  foo->~FooT();

  call_deallocate(allocator, memory1);

  auto memory2 = call_allocate(allocator, sizeof(FooN42), alignof(FooN42));
  auto fooN42 = new (memory2) FooN42();

  CHECK_EQ(1, allocator.Tracker().num_blocks());
  CHECK_EQ(align::get_aligned_size(sizeof(FooN42), alignof(FooN42)),
           allocator.Tracker().num_bytes());

  CHECK_EQ(42, fooN42->m_data.size());
}

TEST_CASE("allocate_after_deallocate_several") {
  PassThroughAllocator pass_through;
  DiagnosticAllocator<PassThroughAllocator> allocator(pass_through);

  using FooInt = FooT<int>;
  using FooN42 = FooNT<std::string, 42>;

  auto memory1 = call_allocate(allocator, sizeof(FooInt), alignof(FooInt));
  auto foo42 = new (memory1) FooInt("Foo42", 42);

  auto memory2 = call_allocate(allocator, sizeof(FooN42), alignof(FooN42));
  auto fooN42 = new (memory2) FooN42();

  auto memory3 = call_allocate(allocator, sizeof(FooInt), alignof(FooInt));
  auto foo42_3 = new (memory3) FooInt("Foo42_3", 423);

  auto memory4 = call_allocate(allocator, sizeof(FooN42), alignof(FooN42));
  auto fooN42_4 = new (memory4) FooN42();

  // deallocations
  fooN42->~FooN42();
  call_deallocate(allocator, memory2);

  foo42_3->~FooInt();
  call_deallocate(allocator, memory3);

  // one additional allocation.
  auto memory5 = call_allocate(allocator, sizeof(FooInt), alignof(FooInt));
  auto foo42_5 = new (memory5) FooInt("Foo42_5", 425);

  // 3 instances - two of one type, one of another
  CHECK_EQ(3, allocator.Tracker().num_blocks());
  int64_t size = align::get_aligned_size(sizeof(FooInt), alignof(FooInt)) * 2 +
                 align::get_aligned_size(sizeof(FooN42), alignof(FooN42));
  CHECK_EQ(size, allocator.Tracker().num_bytes());

  CHECK_EQ("Foo42", foo42->m_name);
  CHECK_EQ(42, foo42->m_data);

  CHECK_EQ(42, fooN42_4->m_data.size());

  CHECK_EQ("Foo42_5", foo42_5->m_name);
  CHECK_EQ(425, foo42_5->m_data);
}
