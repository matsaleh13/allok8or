project (allok8or-core)

debug_boilerplate()

file(GLOB headers ${PROJECT_SOURCE_DIR}/*.h ${PROJECT_SOURCE_DIR}/*/*.h)
file(GLOB cppfiles ${PROJECT_SOURCE_DIR}/*.cpp ${PROJECT_SOURCE_DIR}/*/*.cpp)

add_library(allok8or-core STATIC ${headers} ${cppfiles})
set_target_properties(allok8or-core PROPERTIES LINKER_LANGUAGE CXX)

# Export for access by others.
set(alloc8or_core_include ${PROJECT_SOURCE_DIR} PARENT_SCOPE)

