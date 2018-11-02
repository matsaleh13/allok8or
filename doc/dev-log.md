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


