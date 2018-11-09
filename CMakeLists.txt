cmake_minimum_required(VERSION 3.0.0)
project(allok8or VERSION 0.1.0)

# Macros
include("./macros.cmake")

debug_platform_info()
debug_boilerplate()

# Compiler settings.
set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
warnings_strict()

# Add sub-project folders
add_subdirectory(src)

# Test setup
include(CTest)
add_subdirectory(test)

set(CPACK_PROJECT_NAME ${PROJECT_NAME})
set(CPACK_PROJECT_VERSION ${PROJECT_VERSION})
include(CPack)


