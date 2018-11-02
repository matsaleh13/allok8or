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


