# allok8or Development Log

## 2018-11-01

- Initial repo setup and commit.
- Started project config for CMake.

## 2018-11-02

- Continued project config with CMake.
- Researched using CTest with [doctest.h](https://github.com/onqtam/doctest).
- Implemented align.h with utils for alignment supporting operations.
- Implemented first tests, for aligh.h, using doctest.h.
- All tests pass; committed.
- Implemented PageAllocator in page.h/cpp.
- Also tests for PageAllocator.
- Got a linker error in the PageAllocator test. Turns out I needed to add link directives to the CMakeLists.txt:

    ```CMake
    target_link_libraries(page_allocator-test allok8or-core)
    ```
- All tests pass. Committed.

## 2018-11-06

- Got project to build on OS X with clang:
  - Minor changes to compiler settings to back off pedantic/extra warnings in "strict" mode.
  - Removed invalid uses of `constexpr` that clang caught but MSVS did not.
  - Platform-specific implmementations of aligned malloc.
  - Other minor platform-specific tweaks.
- Back on Windows, got it all building correctly again (not much more to do):
  - However, the VS Code intellisense thingy reported red squigglies in `pass_through.h` under `namespace allok8or`, with the message:
    ```c++
    expected ';' after top level declarator
    ```
  - I recalled that I'd updated my VS Code extensions on OS X to add the vscode-clangd extension. Some googles told me this error was related to that, so I disabled it on Windows.
  - That got rid of the squigglies. Is clang not playing nicely with MSVS on Windows?

## 2018-11-07

- Implemented DiagnosticAllocator (no tests yet).
- Implemented AllocationTracker and related code (no tests yet).
- Compiles on Windows, but templates not expanded I'm sure. 

## 2018-11-08

- Fixed clang build errors of AllocationTracker on OSX.
- Implemented AllocationTracker tests on Windows.
- Iterated on test/debug/fix for a while.
- All tests pass on Windows.
- Committed, pushed up, and pulled onto OSX.
- Build failures, damn. Clang and MSVS are *so* different, feh.
  ```cmake
  [build] In file included from /Users/matsaleh/Dev/github/allok8or/test/diagnostic_util-test.cpp:8:
  [build] /Users/matsaleh/Dev/github/allok8or/src/diagnostic_util.h:32:27: error: array is too large (18446744073709551577 elements)
  [build]     static char type_name[size] = {};
  [build]                           ^~~~
  ```
