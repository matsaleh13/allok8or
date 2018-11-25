# allok8or TODO List

## Features

- Synchronization policies:
  - Thread-unsafe.
  - Thread-safe (locking/critsec).
  - Lock-free (maybe).
- Add actual tracking and logging to `diagnostic::AllocationTracker`:
  - Map-like structure to group by data type.
- Allocator Types:
  - Block allocator (i.e. fixed-size-block pool allocator).
  - Linear allocator.
  - Stack allocator (maybe, but only if I can think of a realistic use case).
  - Free List allocator (i.e. variable size block pool allocator). NOTE: ROI on this may not be worth it. If it's easy enough to use multiple block allocators, we may not need it.
  - NOTE: for all the above, special care needed to provide a way to maximize locality and/or provide a way to do so via the API.

## Tech Debt

- More test cases for logging-test.
- Linker error with logging-test on clang/OSX:

  ```cmake
  [build] Undefined symbols for architecture x86_64:
  [build]   "std::__1::basic_ostream<char, std::__1::char_traits<char> >& std::__1::operator<<<char, std::__1::char_traits<char>, std::__1::allocator<char> >(std::__1::basic_ostream<char, std::__1::char_traits<char> >&, std::__1::basic_string<char, std::__1::char_traits<char>, std::__1::allocator<char> > const&)", referenced from:
  [build]       doctest::String doctest::detail::stringifyBinaryExpr<std::__1::basic_string<char, std::__1::char_traits<char>, std::__1::allocator<char> >, std::__1::basic_string<char, std::__1::char_traits<char>, std::__1::allocator<char> > >(std::__1::basic_string<char, std::__1::char_traits<char>, std::__1::allocator<char> > const&, char const*, std::__1::basic_string<char, std::__1::char_traits<char>, std::__1::allocator<char> > const&) in logging-test.cpp.o
  [build] ld: symbol(s) not found for architecture x86_64
  [build] clang: error: linker command failed with exit code 1 (use -v to see invocation)
  ```