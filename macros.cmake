
macro(debug_boilerplate)
    debug_message("CMAKE_SOURCE_DIR: ${CMAKE_SOURCE_DIR}")
    debug_message("CMAKE_BINARY_DIR: ${CMAKE_BINARY_DIR}")
    debug_message("PROJECT_SOURCE_DIR: ${PROJECT_SOURCE_DIR}")
    debug_message("PROJECT_BINARY_DIR: ${PROJECT_BINARY_DIR}")
endmacro(debug_boilerplate)

macro(debug_message msg)
    set(DBG_LABEL "DBG (${PROJECT_NAME})")
    message ("${DBG_LABEL}: ${msg}")
endmacro(debug_message)

macro(warnings_strict)
    debug_message("Enabling strict compiler warnings.")
    if (MSVC)
        # warning level 4 and all warnings as errors
        add_compile_options(/W4 /WX)
    else()
        # lots of warnings and all warnings as errors
        # add_compile_options(-Wall -Wextra -pedantic -Werror)
        add_compile_options(-Wall -Werror)
    endif()
endmacro(warnings_strict)

macro(warnings_moderate)
    debug_message("Enabling moderate compiler warnings.")
    if (MSVC)
        # warning level 3 and all warnings as errors
        add_compile_options(/W3 /WX)
    else()
        # major warnings and all warnings as errors
        add_compile_options(-W3 -Werror)
    endif()
endmacro(warnings_moderate)

macro(warnings_relaxed)
    debug_message("Enabling relaxed compiler warnings.")
    if (MSVC)
        # warning level 3 and no warnings as errors
        add_compile_options(/W3)
    else()
        # major warnings and no warnings as errors
        add_compile_options(-W3)
    endif()
endmacro(warnings_relaxed)