# allok8or TODO List

## Features

- Synchronization policies:
  - Thread-unsafe.
  - Thread-safe (locking/critical section).
  - Lock-free (maybe).
- ~~Add actual tracking and logging to `diagnostic::AllocationTracker`:~~
  - ~~Map-like structure to group by data type.~~
  - ~~CSV-formatted output.~~
  - Text output (i.e. for screen/readable report).
  - Sorting output by property (e.g. type, count, bytes).
- Allocator Types:
  - Block allocator (i.e. fixed-size-block pool allocator).
  - Linear allocator.
  - Stack allocator (maybe, but only if I can think of a realistic use case).
  - Free List allocator (i.e. variable size block pool allocator). NOTE: ROI on this may not be worth it. If it's easy enough to use multiple block allocators, we may not need it.
  - NOTE: for all the above, special care needed to provide a way to maximize locality and/or provide a way to do so via the API.

## Enhancements



## Tech Debt

- Use real std:: short-form integer type names (e.g. uint_t) instead of my own.
- Namespace `diagnostic`:
  - Restructure files under a `diagnostic` folder and shorten names.
- Pass-through allocator:
  - Rename to "heap allocator" or "system allocator"?
  - Replace conditional compiled calls to OS-specific allocation APIs with a template param that takes a struct with platform-specific implementations for alloc/dealloc calls.
- More test cases for logging-test.
- Linker error with logging-test on clang/OSX:

  ```cmake
  [build] Undefined symbols for architecture x86_64:
  [build]   "std::__1::basic_ostream<char, std::__1::char_traits<char> >& std::__1::operator<<<char, std::__1::char_traits<char>, std::__1::allocator<char> >(std::__1::basic_ostream<char, std::__1::char_traits<char> >&, std::__1::basic_string<char, std::__1::char_traits<char>, std::__1::allocator<char> > const&)", referenced from:
  [build]       doctest::String doctest::detail::stringifyBinaryExpr<std::__1::basic_string<char, std::__1::char_traits<char>, std::__1::allocator<char> >, std::__1::basic_string<char, std::__1::char_traits<char>, std::__1::allocator<char> > >(std::__1::basic_string<char, std::__1::char_traits<char>, std::__1::allocator<char> > const&, char const*, std::__1::basic_string<char, std::__1::char_traits<char>, std::__1::allocator<char> > const&) in logging-test.cpp.o
  [build] ld: symbol(s) not found for architecture x86_64
  [build] clang: error: linker command failed with exit code 1 (use -v to see invocation)
  ```