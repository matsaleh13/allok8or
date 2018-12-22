project (allok8or-test)

debug_boilerplate()
debug_message("alloc8or_core_include: ${alloc8or_core_include}")
include_directories(${alloc8or_core_include})

file(GLOB headers ${PROJECT_SOURCE_DIR}/*.h)
file(GLOB cppfiles ${PROJECT_SOURCE_DIR}/*.cpp)

# Single shared library for all tests.
add_library(allok8or-test OBJECT main.cpp)

# Separate executable for each test (allows for parallel execution).
add_executable(align-test align-test.cpp $<TARGET_OBJECTS:allok8or-test>)
add_test(NAME align-test COMMAND align-test)

add_executable(page_allocator-test page_allocator-test.cpp $<TARGET_OBJECTS:allok8or-test>)
add_test(NAME page_allocator-test COMMAND page_allocator-test)
target_link_libraries(page_allocator-test allok8or-core)

add_executable(pass_through_allocator-test pass_through_allocator-test.cpp $<TARGET_OBJECTS:allok8or-test>)
add_test(NAME pass_through_allocator-test COMMAND pass_through_allocator-test)
target_link_libraries(pass_through_allocator-test allok8or-core)

add_executable(diagnostic_tracking_pool-test diagnostic_tracking_pool-test.cpp $<TARGET_OBJECTS:allok8or-test>)
add_test(NAME diagnostic_tracking_pool-test COMMAND diagnostic_tracking_pool-test)
target_link_libraries(diagnostic_tracking_pool-test allok8or-core)

add_executable(diagnostic_util-test diagnostic_util-test.cpp $<TARGET_OBJECTS:allok8or-test>)
add_test(NAME diagnostic_util-test COMMAND diagnostic_util-test)
target_link_libraries(diagnostic_util-test allok8or-core)

add_executable(type_name_helper-test type_name_helper-test.cpp $<TARGET_OBJECTS:allok8or-test>)
add_test(NAME type_name_helper-test COMMAND type_name_helper-test)
target_link_libraries(type_name_helper-test allok8or-core)

add_executable(diagnostic_allocator-test diagnostic_allocator-test.cpp $<TARGET_OBJECTS:allok8or-test>)
add_test(NAME diagnostic_allocator-test COMMAND diagnostic_allocator-test)
target_link_libraries(diagnostic_allocator-test allok8or-core)

add_executable(diagnostic_block_header-test diagnostic_block_header-test.cpp $<TARGET_OBJECTS:allok8or-test>)
add_test(NAME diagnostic_block_header-test COMMAND diagnostic_block_header-test)
target_link_libraries(diagnostic_block_header-test allok8or-core)

add_executable(diagnostic_allocation_stats-test diagnostic_allocation_stats-test.cpp $<TARGET_OBJECTS:allok8or-test>)
add_test(NAME diagnostic_allocation_stats-test COMMAND diagnostic_allocation_stats-test)
target_link_libraries(diagnostic_allocation_stats-test allok8or-core)

add_executable(diagnostic_allocation_stats_reporter-test diagnostic_allocation_stats_reporter-test.cpp $<TARGET_OBJECTS:allok8or-test>)
add_test(NAME diagnostic_allocation_stats_reporter-test COMMAND diagnostic_allocation_stats_reporter-test)
target_link_libraries(diagnostic_allocation_stats_reporter-test allok8or-core)

add_executable(global_allocator-test global_allocator-test.cpp $<TARGET_OBJECTS:allok8or-test>)
add_test(NAME global_allocator-test COMMAND global_allocator-test)
target_link_libraries(global_allocator-test allok8or-core)

add_executable(logging-test logging-test.cpp $<TARGET_OBJECTS:allok8or-test>)
add_test(NAME logging-test COMMAND logging-test)
target_link_libraries(logging-test allok8or-core)

add_executable(std_allocator_adapter-test std_allocator_adapter-test.cpp $<TARGET_OBJECTS:allok8or-test>)
add_test(NAME std_allocator_adapter-test COMMAND std_allocator_adapter-test)
target_link_libraries(std_allocator_adapter-test allok8or-core)

