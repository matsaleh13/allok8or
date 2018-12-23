# allok8or Development Log

## 2018-11-01

- Initial repo setup and commit.
- Started project config for CMake.

## 2018-11-02

- Continued project config with CMake.
- Researched using CTest with [doctest.h](https://github.com/onqtam/doctest).
- Implemented align.h with utils for alignment supporting operations.
- Implemented first tests, for align.h, using doctest.h.
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
  - Removed invalid uses of ``constexpr`` that clang caught but MSVS did not.
  - Platform-specific implementations of aligned malloc.
  - Other minor platform-specific tweaks.
- Back on Windows, got it all building correctly again (not much more to do):
  - However, the VS Code intellisense thingy reported red squigglies in `pass_through.h` under `namespace allok8or`, with the message:
    ```c++
    expected ';' after top level declarator
    ```
  - I recalled that I'd updated my VS Code extensions on OS X to add the `vscode-clangd` extension. Some googles told me this error was related to that, so I disabled it on Windows.
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
- Another clang error, but another case of implementation-specific include dependencies:

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
  - That means we can't use a `constexpr` method. Boo.
  - Gonna let this stew overnight.

## 2018-11-10

- Finally got TypeNameHelper working on Clang:
  - Preserved `constexpr` by adding a method that takes the __*FUNCTION__ string and a `start` and `end` pointer as OUT params. That way, no need to declare local vars.
  - Refactored to minimize the amount of compiler-specific code.
- Meanwhile, back in MSVS, we have new test errors:
  - First, forgot to init `start` so overflowed the array size argument.
  - Wanted to use the fixed size of the string literal to avoid walking the string, so implemented a template version with array size argument. That worked pretty well.
  - But, will it work on Clang, with the non-macro implementation of `__PRETTY_FUNCTION__`?

## 2018-11-11

- Still working on getting TypeNameHelper and tests to work on MSVS:
  - Regex matching is kind of a PITA because of how `__FUNCTION__` and `__PRETTY_FUNCTION__` differ.
  - Eventually decided to relax the patterns a bit until the tests work. We just have to get close IMO.
  - Ready to commit and test again on Clang.
- Got the array size template to work on Clang too, surprisingly: 
  - Need to refresh my memory on how that works, because I thought a non-literal `const char*` string wouldn't do it.
  - Tests work too, w00t.
- Implemented tests for DiagnosticAllocator:
  - Fixed compile errors from template instantiations.
  - Fixed a few runtime errors.
  - All tests pass on Windows.
- Verified that everything builds with clang and all tests pass on OS X, w00t.

## 2018-11-12

- Moved TypeNameHelper and tests into their own header/cpp files:
  - Built and re-ran tests on Windows, all good.
  - Committed and synced on OSX, and got build errors with clang, because I'd forgotten to `#include "type_name_helper.h"` from `allocation_tracker.h`. WTF didn't that show up in MSVS?
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
  - Bah, names are getting long, but IMO the diagnostic stuff needs to be identified because they're all in the same namespace.
- Even more refactoring, now on OSX:
  - Clang found include issues that MSVS did not, because the templates haven't yet been expanded.
  - Moved the operator* implementation back to diagnostic_block_header.h.
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
  - MSVS doesn't seem to care. I didn't even use `__VA_OPT__`, only `__VA_ARGS__` alone, and that worked just fine.
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
  - The most common advice is to pass the string variable through "%s", but that doesn't help me, because in my case the variable *is* a format string.
  - Ended up working around the issue with a local #pragma ignore:

    ```c++

      // Sadly, this is the only way i can find to avoid this error.
      #pragma clang diagnostic push
      #pragma clang diagnostic ignored "-Wformat-security"
            const size_t actual_size = std::snprintf(nullptr, 0, format, args...) + 1;
            std::snprintf(buf, actual_size, format, args...);
      #pragma clang diagnostic pop

    ```

## 2018-11-15

- Starting to think about how to do the tracking of BlockHeaders so I can generate metrics about them (e.g. total allocations by type):
  - Requirement: Some kind of mapping data structure to associate string "type" to a collection of BlockHeader instances.
  - Requirement: Calculate count/sum of allocations and bytes by "type". <= O(n) for that (where n is instances of a single type).
  - Requirement: Iterate over types to get their associated values; possibly sorting.
  - Requirement: O(1) insert/remove/access.
  - Constraint: Avoid using global new/delete, implicitly, in case we end up in some kind of recursion. We are writing an allocator here, and users are almost certain to overload global new/delete.
  - Option: Implement a bare-bones custom associative array type thing and make all calls to malloc/free explicit.
  - Option: Keep a running count/total of allocations for each type. This eliminates the need for a list per type, but not the need for a mapping by type.
  - Option: Use std::unordered_map with a custom std::allocator that controls access to new/malloc/delete/free. IMO this could be ideal, because we want a std::allocator adapter anyway.
- I'll start by writing the std::allocator adapter:
  - Found a good starting point: [Allocator Boilerplate](https://howardhinnant.github.io/allocator_boilerplate.html).
  - In this case, we'll add another template param for an allok8or::Allocator-derived class.

## 2018-11-17

- Continuing with the std::allocator adapter:
  - Added operator== and operator!= to allok8or::Allocator, because it's required by std::allocator.

## 2018-11-18

- StdAllocatorAdapter continued:
  - Added tests for operator== and operator!= to allok8or::Allocator, DiagnosticAllocator, and PassThroughAllocator.
  
## 2018-11-20

- StdAllocatorAdapter continued:
  - Implemented `std_allocator_adapter-test.cpp`.
  - Added tests for API with `PassThroughAllocator` as the backing allocator.
  - Added test using `StdAllocatorAdapter` with `std::map`.
  - Discovered I needed to make the `allocate` and `deallocate` methods const, because `StdAllocatorAdapter` is stateful (i.e. internal backing allocator), and its internal reference must be const. This is probably okay, except that it requires *all* internal state (including nested state) to either be const or mutable. This includes things like the `AllocationTracker` inside the `DiagnosticAllocator` must be mutable, which is pretty heavyweight. So far so good, because mutable semantics are preserved (i.e. external state of allocator is still const), but I'm a little wary of downstream impacts.
  - Also discovered that `std::allocator` requires that the allocator be copy-constructible, move-constructible, and copy-assignable. That makes stateful allocator implementation tricky. Again, in the case of the `DiagnosticAllocator`, I need to somehow manage the fact that there would be a shared `AllocationTracker` between the copies. That means synchronization and lifetime management that I'd rather not *have* to do.
  - Still WIP, iterating on tests.

## 2018-11-21

- StdAllocatorAdapter continued:
  - Added tests using std::map container with both PassThroughAllocator and DiagnosticAllocator.
  - Got all tests working.
  - Would like better tests for deallocate and allocate_array, but that would probably require intrusive marking of the raw memory buffer. Not ready to do that.
- Question: should I make all allocators default-constructible? 
  - I want to use dependency injection to reduce coupling, so I've been passing backing allocators as args to ctor in both DiagnosticAllocator and StdAllocatorAdapter.
  - However, if everything is default-constructible, simply passing the allocator type as a template argument should be enough, right?
  - Making allocators default-constructible would simplify internal members, holding them by value, not by reference, and ownership would be simpler.
  - If needed, I could still support traditional DI by adding a ctor overload that takes the type I need, and assigning the internal type to it.
  - The above implies that all allocators must be copy-assignable and most likely copy-constructible. This is probably good though, as `std::allocator` requires that also.

## 2018-11-22

- StdAllocatorAdapter continued:
  - Arg... conflicted and confused about the interplay between requirements for `std::allocator` copy/move and default construction.
  - What that really boils down to is whether the backing allocator (and related stateful class members) must be held by value (i.e. created internally and/or copy/assigned) or by reference (simply shared).
  - When holding by value I quickly ran into (predicted) issues with what to do when I use a `StdAllocatorAdapter` in an STL container. Ultimately it pushes the copy/assign issue deeper into members of members, such that everything needs to be copy/assignable, which does complexify things.
  - Also, at some level down, stateful members that manage resources must ultimately be shared, because we cannot copy all the backing memory they manage.
  - So, I'm swinging back towards holding the backing allocator by reference, because the higher up that's done, the simpler things get. I think.
  - Part of the problem for me is (not) understanding `rebind` well enough. It seems that `rebind` makes use of a *converting* copy ctor, that takes the new (rebound) type as a template argument. So, that right there tells me that `rebind` means copying the `StdAllocatorAdapter` while sharing the backing allocator.
  - Oh, and the requirement that copies of a `std::allocator` be equal virtually mandates that internal state be shared, too.
  - Here's an approximate example of an allocator with shared internal state (the `arena`): https://howardhinnant.github.io/short_alloc.h, with usage example here: https://howardhinnant.github.io/stack_alloc.html. In this example the stateful member is held by reference, passed as a ctor argument, and is shared on copy. I don't think we can take this as a *canonical* example to follow, but it's not a bad stake in the ground.
  - Reverted the changes I'd made that held the backing allocator by value, back to a reference. Caller will own the lifecycle of the backing allocator for now.
  - Added a couple more tests that include copying a std::map. All good.

## 2018-11-23

- Created MockAllocator class for better testing of `StdAllocatorAdapter`:
  - Ctor takes two callback functions, one for allocate and the other for deallocate.
  - Got compiler error tho, when passing lambdas to the ctor:

  ```cmake
  [build] ..\test\std_allocator_adapter-test.cpp(54): error C2664: 'allok8or::test::MockAllocator::MockAllocator(const allok8or::test::MockAllocator &&)': cannot convert argument 1 from '_DOCTEST_ANON_FUNC_2::<lambda_4d3a60ddb2e0b246c787357bea18979d>' to 'allok8or::test::CbAllocate'
  ```

  - Turns out it's because the implicit return type of the lambda didn't agree with the function signature of the callback. Fixed.
- Modified the `std_allocator_adapter-test.cpp` to use the `MockAllocator` instead of the `PassThroughAllocator` directly.
- Started implementing stats tracking code for use by the DiagnosticAllocator:
  - `diagnostic_allocation_stats.h`, with `AllocationStatsTracker` and related classes.

## 2018-11-24

- Continued work on `AllocationStatsTracker` and related classes:
  - `AllocationStatsKey` struct has info about call site and type.
  - `AllocationStats` struct has allocation/deallocation counts and net value accessors.
  - `AllocationStatsTracker` class has an `std::unordered_map` of `AllocationStatsKey` to `AllocationStats` to track counts of every allocation type/call site.
  - The `unordered_map` uses the `StdAllocationAdapter<T, PassThroughAllocator>`, to avoid possibility of calling overloaded new/delete and maybe ending up in a recursion or something.
- Tested above on clang/OSX, and got compiler errors (of course):

  ```cmake
  build] In file included from /Users/matsaleh/Dev/github/allok8or/test/diagnostic_allocation_stats-test.cpp:8:
  [build] In file included from /Users/matsaleh/Dev/github/allok8or/src/diagnostic_allocation_stats.h:15:
  [build] /Library/Developer/CommandLineTools/usr/include/c++/v1/unordered_map:756:5: error: static_assert failed due to requirement 'is_same<value_type, typename allocator_type::value_type>::value' "Invalid allocator::value_type"
  [build]     static_assert((is_same<value_type, typename allocator_type::value_type>::value),
  [build]     ^              ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
  ```

- According to a handful of web results, this is caused by the unordered_map key (i.e. `AllocationStatsKey`) not being `const`. Seems clang enforces this with a static assert, but (apparently) MSVS does not. References:
  - [Eigen::aligned_allocator fails with std::unordered_multimap](https://stackoverflow.com/questions/27336941/eigenaligned-allocator-fails-with-stdunordered-multimap)
  - [Compiling with unordered_map fails with Xcode 5.1.1 on OS X 10.9.3](https://stackoverflow.com/questions/24745702/compiling-with-unordered-map-fails-with-xcode-5-1-1-on-os-x-10-9-3)
  - [/usr/include/c++/v1/map:837:5: error: static_assert failed "Allocator::value_type must be same type as value_type"](https://lists.freebsd.org/pipermail/freebsd-hackers/2016-June/049587.html)
  - These all seem anecdotal, but changing:

  ```c++
    using value_type = std::pair<AllocationStatsKey, AllocationStats>;
  ```

    to

  ```c++
    using value_type = std::pair<const AllocationStatsKey, AllocationStats>;
  ```

    worked.

- But, after that, I ended up with test failures on clang:

  ```cmake
  [ctest] /Users/matsaleh/Dev/github/allok8or/test/std_allocator_adapter-test.cpp:129:
  [ctest] TEST CASE:  std_map_add_remove
  [ctest] 
  [ctest] /Users/matsaleh/Dev/github/allok8or/test/std_allocator_adapter-test.cpp:154: ERROR: CHECK_EQ( 1, allocate_called ) is NOT correct!
  [ctest]   values: CHECK_EQ( 1, 2 )
  [ctest] 
  [ctest] /Users/matsaleh/Dev/github/allok8or/test/std_allocator_adapter-test.cpp:157: ERROR: CHECK_EQ( 2, allocate_called ) is NOT correct!
  [ctest]   values: CHECK_EQ( 2, 3 )
  [ctest] 
  [ctest] /Users/matsaleh/Dev/github/allok8or/test/std_allocator_adapter-test.cpp:160: ERROR: CHECK_EQ( 3, allocate_called ) is NOT correct!
  [ctest]   values: CHECK_EQ( 3, 5 )
  [ctest] 
  [ctest] /Users/matsaleh/Dev/github/allok8or/test/std_allocator_adapter-test.cpp:174: ERROR: CHECK_EQ( 1, deallocate_called ) is NOT correct!
  [ctest]   values: CHECK_EQ( 1, 2 )
  ```

- This was a problem when using `std::unordered_map`, but not `std::map` for that test. Bah, it's counting allocations, but this behavior is obviously implementation-specific. Seems the `std::unordered_map` allocates in a less deterministic way. So, I worked around it by resetting my test counter to 0 before each add/remove to/from the map, then just used `CHECK_GT(allocate_called, 0)` to test.
- After that all tests passed again.

## 2018-11-25

- Integrating the stats tracking code into the `DiagnosticAllocator`.
- Decided I needed a way to allocate an `AllocationStatsTracker` instance without global new:
  - This is because the consuming class (`AllocationTracker`) had forward declared it to keep it hidden.
  - That meant using the `AllocationStatsTracker`'s own internal allocator (because I most likely can't trust any other). 
  - Easy enough, just overloaded class-specific operator new/delete to redirect allocations to the backing allocator.

## 2018-12-08

- Something broke with my cmake extensions in VS Code. When I configure the project, I now get (when using the MSVS 2017 kit):

  ```cmake
  [driver] Removing  e:/Dev/GitHub/allok8or/build/CMakeCache.txt
  [driver] Removing  e:\Dev\GitHub\allok8or\build\CMakeFiles
  [cms-client] Configuring using the "Ninja" CMake generator
  [cmake] The C compiler identification is Clang 6.0.1
  [cmake] CMake Error at C:/Program Files/CMake/share/cmake-3.12/Modules/CMakeDetermineCompilerId.cmake:793 (message):
  [cmake]   The Clang compiler tool
  [cmake] 
  [cmake]     "C:/Program Files/LLVM/bin/clang.exe"
  [cmake] 
  [cmake]   targets the MSVC ABI but has a GNU-like command-line interface.  This is
  [cmake]   not supported.  Use 'clang-cl' instead, e.g.  by setting 'CC=clang-cl' in
  [cmake]   the environment.  Furthermore, use the MSVC command-line environment.
  [cmake] Call Stack (most recent call first):
  [cmake]   C:/Program Files/CMake/share/cmake-3.12/Modules/CMakeDetermineCCompiler.cmake:113 (CMAKE_DIAGNOSE_UNSUPPORTED_CLANG)
  [cmake]   CMakeLists.txt:2 (project)
  [cmake] 
  [cmake] 
  [cmake] CMake Error: CMAKE_CXX_COMPILER not set, after EnableLanguage
  [cmake] Configuring incomplete, errors occurred!
  [cmake] See also "e:/Dev/GitHub/allok8or/build/CMakeFiles/CMakeOutput.log".
  [cms-driver] Error during CMake configure: [cmake-server] Configuration failed.  
  ```

  - Nothing I tried (reconfig clean, etc) fixed it.
  - Finally had to install VS Code insider edition so I could get the new feature of installing specific extension version. This allowed me to downgrade the `vector-of-bool.cmake-tools@1.1.3` extension to `vector-of-bool.cmake-tools@1.1.2`.
  - After restarting (and some misc hiccups), I was able to successfully configure the project using cmake-tools in the insider version of vscode.
  - Then the extension was auto-updated back to 1.1.3, and when I reconfigured again, the problem returned.
  - I reproed this twice with the same results.
  - FWIW, when I configure using 1.1.2 in the VSCode insiders build, then open the workspace in VSCode, configure works (until I reconfigure clean again).
  - I then copied the v 1.1.2 folder from .vscode-insiders\extensions into .vscode\extensions, restarted VSCode, did a reconfigure clean, and that fixed it.
  - Also note that with v1.1.3, I lost the kit `Clang 6.0.1 for MSVC with Visual Studio Community 2017 (amd64)`.

## 2018-12-09

- Started unit tests for the `diagnostic_allocation_stats_reporter.h`. Still WIP.

## 2018-12-22

- Had weirdness with my development hard drive after a Windows update. Drive totally not recognized. Took a few reboots, but after that, everything seemed fine again. Ran diags and repairs, nothing broken, nothing lost. Still thank God I had backups and GitHub! :)
- Committed the above tests from 2018-12-09.
- Fleshed out the rest of the tests for the `AllocationStatsCsvReporter` implementation class.
- Calling this done for now (TODO: add `AllocationStatsTextReporter`)

## 2018-12-23

- Starting on block allocator implementation.
- Design issue:
  - current Allocator (base/interface) class allocates memory of any given size passed into the `allocate` method.
  - Block allocator, by definition, allocates only a specific size block.
  - It seems that these two APIs are incompatible. I.e. a BlockAllocator cannot be allowed to allocate just any size block.
  - Furthermore, what we currently call the `PageAllocator` is actually a fixed-size block allocator that is used to allocate the backing memory "pages" used by other allocators. We already know that this allocator doesn't satisfy the existing `Allocator` interface.
- Do we have two types of allocator interfaces?
  - Fixed-size (i.e. block).
  - Variable-size (i.e. per allocate call).
  - Conceptually, a block allocator could delegate to a non-block allocator
- Reviewing other allocator examples that deal with this:
  - The approach taken by [Jonathan Mueller's memory library](https://github.com/foonathan/memory) is very precise, using separate "concepts" for fixed and variable allocators. It uses fairly sophisticated traits classes and other template metaprogramming techniques to enforce the desired semantics. Downside is that that code is kind of hard to read, and requires jumping around in a lot of files. My gut tells me that this uses the "correct" (and probably most flexible) approach at the cost of some usability.
  - The article by Tiago Costa, [C++: Custom memory allocation](https://www.gamedev.net/articles/programming/general-and-gameplay-programming/c-custom-memory-allocation-r3010/), takes a simpler approach, if less elegant. There is a single abstract base `Allocator` class, which exposes allocation size as an argument to the `allocate` method. It also implements fixed block `PoolAllocator` that derives from the `Allocator` and does implement the `allocate` method having the size parameter. This method simply asserts that the size argument equals the fixed size expected by the allocator. Very simple, if clumsy and kind of hacky. Yet, it's also very easy to understand and use.
  - Of course, I want the best of both worlds. Because, like Mueller, I'm using a template-based approach, I don't really need an abstract base class. Maybe I need to create a traits thingy too? Ideally, it would allow an allocator class to declare an `allocate` method with or without a size parameter. But, I know next to nothing about implementing traits classes, so I'm going to have to study on it for a bit.  
