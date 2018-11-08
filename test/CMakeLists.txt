project (allok8or-test)

debug_boilerplate()
debug_message("alloc8or_core_include: ${alloc8or_core_include}")
include_directories(${alloc8or_core_include})

file(GLOB headers ${PROJECT_SOURCE_DIR}/*.h)
file(GLOB cppfiles ${PROJECT_SOURCE_DIR}/*.cpp)

# Single shared library for all tests.
add_library(allok8or-test OBJECT main.cpp)

# Separate executable for each test.
add_executable(align-test align-test.cpp $<TARGET_OBJECTS:allok8or-test>)
add_test(NAME align-test COMMAND align-test)

add_executable(page_allocator-test page_allocator-test.cpp $<TARGET_OBJECTS:allok8or-test>)
add_test(NAME page_allocator-test COMMAND page_allocator-test)
target_link_libraries(page_allocator-test allok8or-core)

add_executable(pass_through_allocator-test pass_through_allocator-test.cpp $<TARGET_OBJECTS:allok8or-test>)
add_test(NAME pass_through_allocator-test COMMAND pass_through_allocator-test)
target_link_libraries(pass_through_allocator-test allok8or-core)

add_executable(allocation_tracker-test allocation_tracker-test.cpp $<TARGET_OBJECTS:allok8or-test>)
add_test(NAME allocation_tracker-test COMMAND allocation_tracker-test)
target_link_libraries(allocation_tracker-test allok8or-core)

add_executable(diagnostic_util-test diagnostic_util-test.cpp $<TARGET_OBJECTS:allok8or-test>)
add_test(NAME diagnostic_util-test COMMAND diagnostic_util-test)
target_link_libraries(diagnostic_util-test allok8or-core)