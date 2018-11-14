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

## 2018-11-09

- Fixed clang build error from yesterday:
  - In spite of web info to the contrary, it seems `__FUNCTIION__` isn't defined by clang (on OSX High Sierra, on my machine, anyway).
  - The oversized value for array size was a negative value assigned to a size_t and overflowing (a reminder about the dangers of unsigned int).
  - Fixed by defining custom macro `ALK8_PRETTY_FUNCTION` and set to either `__FUNCTION__` or `__PRETTY_FUNCTION__` based on compiler defines.
- Another clang error, but another case of implementation-specific include depenencies:

  ```cmake
    build] /Users/matsaleh/Dev/github/allok8or/src/diagnostic_util.h:37:5: error: call to function 'memcpy' that is neither visible in the template definition nor found by argument-dependent lookup
    [build]     memcpy(type_name, ALK8_PRETTY_FUNCTION + FRONT_SIZE, type_name_size - 1u);
    [build]     ^
  ```

  - Needed `#include <cstring>`, which must have been "inherited" via another header on MSVS, but not clang.
  - Another reminder of the virtue of "include what you use" (if only I knew where/when).
  - Is this an argument for a common include file (possibly precompiled header)?

- Looks like __PRETTY_FUNCTION__ has *two* occurrences of the type name in it:
  - That changes the entire approach. 
  - Now, instead of having a constant prefix and suffix length, we have no way to know how far to walk the string to find the end of the token we're looking for.
  - That means we can't use a constexpr method. Boo.
  - Gonna let this stew overnight.

## 2018-11-10

- Finally got TypeNameHelper working on Clang:
  - Preserved constexpr by adding a method that takes the __*FUNCTION__ string and a `start` and `end` pointer as OUT params. That way, no need to declare local vars.
  - Refactored to minimize the amount of compiler-specific code.
- Meanwhile, back in MSVS, we have new test errors:
  - First, forgot to init `start` so overflowed the array size argument.
  - Wanted to use the fixed size of the string literal to avoid walking the string, so implemented a template version with array size argument. That worked pretty well.
  - But, will it work on Clang, with the non-macro implementation of `__PRETTY_FUNCTION__`?

## 2018-11-11

- Still working on getting TypeNameHelper and tests to work on MSVS:
  - Regex matching is kind of a PITA becuase of how `__FUNCTION__` and `__PRETTY_FUNCTION__` differ.
  - Eventually decided to relax the patterns a bit until the tests work. We just have to get close IMO.
  - Ready to commit and test again on Clang.
- Got the array size template to work on Clang too, surprisingly: 
  - Need to refresh my memory on how that works, because I thought a non-literal const char* string wouldn't do it.
  - Tests work too, w00t.
- Implemented tests for DiagnosticAllocator:
  - Fixed compile errors from template instantiations.
  - Fixed a few runtime errors.
  - All tests pass on Windows.
- Verified that everything builds with clang and all tests pass on OS X, w00t.

## 2018-11-12

- Moved TypeNameHelper and tests into their own header/cpp files:
  - Built and re-ran tests on Windows, all good.
  - Committed and synced on OSX, and got build errors with clang, becuase I'd forgotten to `#include "type_name_helper.h"` from `allocation_tracker.h`. WTF didn't that show up in MSVS?
  - Fixed and committed again.
- A bit of refactoring:
  - Moved the magic macro out of `diagnostic_util.h` and into it's own header, `diagnostic_new.h`. Probably will overload operator new there too.
  - Moved `CallerDetails` into `diagnostic_util.h` from `allocation_tracker.h`.
- Fixed a bug in TypeNameHelper with trailing whitespace.
- Added tests for BlockHeader::set_caller_details:
  - Currently part of the BlockHeader create test, but that smells wrong.
- More refactoring:
  - Moved BlockHeader into `diagnostic_block_header.h`.
  - Moved BlockHeader tests in to `diagnostic_header-test.cpp`
  - Renamed `allocation_tracker` to `diagnostic_allocation_tracker`.
  - Bah, names are getting long, but IMO the diagnostic stuff needs to be identified becuase they're all in the same namespace.
- Even more refactoring, now on OSX:
  - Clang found include issues that MSVS did not, becuase the templates haven't yet been expanded.
  - Moved the operator* impelementation back to diagnostic_block_header.h.
- Created separate tests for `set_caller_details`, including a couple negative paths.

## 2018-11-13

- Want to implement some basic non-singleton way to support global allocator, for use from global operator new overload:
  - Don't want vtable overhead, so no abstract base.
  - But, templated class requires knowing the type always, right?
- Researched various issues related to vtable runtime performance and alternatives:
  - [Curiously-recurring Template Pattern (CRTP)](https://en.wikipedia.org/wiki/Curiously_recurring_template_pattern)
  - [The Curiously Recurring Template Pattern in C++](https://eli.thegreenplace.net/2011/05/17/the-curiously-recurring-template-pattern-in-c/)
  - [The cost of dynamic (virtual calls) vs. static (CRTP) dispatch in C++](https://eli.thegreenplace.net/2013/12/05/the-cost-of-dynamic-virtual-calls-vs-static-crtp-dispatch-in-c)
  - And others.
- Key takeaway for me:
  - Using CRTP allows us to access via the base class if we know the derived class type.
  - At first seems non-abstract/non-polymorphic.
  - But, if we implement APIs as template functions having arguments expressed in terms of `Base<Derived>`, then we get abstract-ish behavior.
  - Also, even though the base class is templated, if the derived class is not, we can put implementation into the .cpp file.
- Implemented a CRTP-based Allocator class to define the interface.
  - Derived `PassthroughAllocator` from it.
  - Updated `pass_through_allocator-test.cpp` to access the allocator via the base<T>.
  - Works so far.
  - Verified on OSX, all good.
  - Extended DiagnosticAllocator to use the CRTP interface class.
  - Also good, committed.
- Implemented templated `GlobalAllocator` utility class:
  - Yes, must access all static members via the fully-qualified templated type.
  - I figure it won't be too bad tho, because we can create aliases as needed.
  - Plus, every global use of an allocator will be (most likely) through overloading operator new (global or class), which is really application code at that point. Nothing generic/abstract there.
  - Implemented basic tests in `global_allocator-test.cpp`.
  - Builds and tests pass on Windows, w00t.

## 2018-11-14

- Renamed `diagnostic_header` to `diagnostic_block_header` everywhere, for consistency with other files.
- Found `BlockHeader` implementation code still in `diagnostic_allocation_tracker.cpp`; moved to `diagnostic_block_header.cpp`.
- Implemented basic logging support.
- Lots of compiler errors on Clang from the logging stuff:
  - Basically of the form:

  ```cmake

    [build] /Users/matsaleh/Dev/github/allok8or/src/page.cpp:80:7: error: expected expression
    [build]       LOG_ERROR( "Failed to allocate new page." );
    [build]       ^
    [build] /Users/matsaleh/Dev/github/allok8or/src/logging.h:30:89: note: expanded from macro 'LOG_ERROR'
    [build] #define LOG_ERROR(message, ...)   allok8or::logging::Logger::log(4, message, __VA_ARGS__)
    [build]                                                                                         ^
  ```

  - Random Googling hints that `__VA_OPT__` isn't supported on Clang.
  - In fact, it seems to be supported only since C++20, but [cppreference.com](https://en.cppreference.com/w/cpp/preprocessor/replace) didn't mention that.
  - Elsewhere, another page hints that [Clang doesn't support](https://stackoverflow.com/questions/48045470/portably-detect-va-opt-support) it either.
  - MSVS doesn't seem to care. I didn't even use `__VA_OPT__`, only `__VA_ARGS__` alone, and that worked jsut fine.
  - I see mentions of an ugly "hack" by GCC to add '##' prefix, i.e. `##__VA_ARGS__`, but I'm not sure that works on clang. Ugh.
- BTW, I learned that you can show the preprocessor output using the -E option on the command line:

  ```bash

    pearl:allok8or matsaleh$ clang -E src/page.cpp

  ```

- ... which gave me:

  ```c++

  allok8or::logging::Logger::log(4, "Failed to allocate new page." __VA_OPT__(,) );  

  ```

- OK, after testing, it appears that `##__VA_ARGS__` *does* work for both MSVS and Clang, so maybe I'm okay. Damn, wish they'd said so.
- After that, I got another compiler warning (i.e. error with `-Wall -Werror`):

  ```cmake

    [build] In file included from /Users/matsaleh/Dev/github/allok8or/src/page.cpp:6:
    [build] /Users/matsaleh/Dev/github/allok8or/src/logging.h:107:60: error: format string is not a string literal (potentially insecure) [-Werror,-Wformat-security]
    [build]       const size_t actual_size = std::snprintf(nullptr, 0, format, args...) + 1;
    [build]                                                            ^~~~~~
    [build] /Users/matsaleh/Dev/github/allok8or/src/page.cpp:80:7: note: in instantiation of function template specialization 'allok8or::logging::Logger::log<>' requested here
    [build]       LOG_ERROR( "Failed to allocate new page." );
    [build]       ^
    [build] /Users/matsaleh/Dev/github/allok8or/src/logging.h:31:3: note: expanded from macro 'LOG_ERROR'
    [build]   LOG(allok8or::logging::Logger::error, message, ##__VA_ARGS__)
    [build]   ^
    [build] /Users/matsaleh/Dev/github/allok8or/src/logging.h:19:30: note: expanded from macro 'LOG'
    [build]   allok8or::logging::Logger::log(level, message, ##__VA_ARGS__)
    [build]                              ^
    [build] /Users/matsaleh/Dev/github/allok8or/src/logging.h:107:60: note: treat the string as an argument to avoid this
    [build]       const size_t actual_size = std::snprintf(nullptr, 0, format, args...) + 1;
    [build]                                                            ^
    [build]                                                            "%s", 

  ```

  - Got some help for this in the [Clang docs](http://clang.llvm.org/docs/AttributeReference.html#format-gnu-format).
  - Led there by this SO thread [Correcting “format string is not a string literal” warning](https://stackoverflow.com/questions/36120717/correcting-format-string-is-not-a-string-literal-warning).
  - However, none of that advice actually fixed *my* error.
  - The most common advice is to pass the string variable through "%s", but that doesn't help me, becuase in my case the variable *is* a format string.
  - Ended up working around the issue with a local #pragma ignore:

    ```c++

      // Sadly, this is the only way i can find to avoid this error.
      #pragma clang diagnostic push
      #pragma clang diagnostic ignored "-Wformat-security"
            const size_t actual_size = std::snprintf(nullptr, 0, format, args...) + 1;
            std::snprintf(buf, actual_size, format, args...);
      #pragma clang diagnostic pop

    ```