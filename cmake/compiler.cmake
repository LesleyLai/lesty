# Compiler specific settings

if (compiler_included)
    return()
endif ()
set(compiler_included true)

# Link this 'library' to use the standard warnings
add_library(compiler_options INTERFACE)
add_library(lesty::compiler_options ALIAS compiler_options)


option(LESTY_WARNING_AS_ERROR "Treats compiler warnings as errors" ON)
if (MSVC)
    target_compile_options(compiler_options INTERFACE /W4 "/permissive-")
    target_compile_definitions(compiler_options INTERFACE _CRT_SECURE_NO_WARNINGS)
    if (LESTY_WARNING_AS_ERROR)
        target_compile_options(compiler_options INTERFACE /WX)
    endif ()
elseif (CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
    target_compile_options(compiler_options
            INTERFACE -Wall
            -Wextra
            -Wshadow
            -Wnon-virtual-dtor
            -Wold-style-cast
            -Wcast-align
            -Wunused
            -Woverloaded-virtual
            -Wpedantic
            -Wsign-conversion
            -Wnull-dereference
            -Wdouble-promotion
            -Wformat=2
            )
    if (LESTY_WARNING_AS_ERROR)
        target_compile_options(compiler_options INTERFACE -Werror)
    endif ()

    if ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "Clang")
        target_compile_options(compiler_options INTERFACE -Wno-c99-extensions)
    endif ()

    if ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "GNU")
        target_compile_options(compiler_options
                INTERFACE -Wmisleading-indentation
                -Wduplicated-cond
                -Wduplicated-branches
                -Wlogical-op
                -Wuseless-cast
                )
    endif ()
endif ()

option(LESTY_ENABLE_PCH "Enable Precompiled Headers" OFF)
if (LESTY_ENABLE_PCH)
    target_precompile_headers(compiler_options INTERFACE
            <algorithm>
            <array>
            <vector>
            <string>
            <utility>
            <functional>
            <memory>
            <memory_resource>
            <string_view>
            <cmath>
            <cstddef>
            <type_traits>
            )
endif ()

option(LESTY_USE_ASAN "Enable the Address Sanitizers" OFF)
if (LESTY_USE_ASAN)
    message("Enable Address Sanitizer")
    target_compile_options(compiler_options INTERFACE
            -fsanitize=address -fno-omit-frame-pointer)
    target_link_libraries(compiler_options INTERFACE
            -fsanitize=address)
endif ()

option(LESTY_USE_TSAN "Enable the Thread Sanitizers" OFF)
if (LESTY_USE_TSAN)
    message("Enable Thread Sanitizer")
    target_compile_options(compiler_options INTERFACE
            -fsanitize=thread)
    target_link_libraries(compiler_options INTERFACE
            -fsanitize=thread)
endif ()

option(LESTY_USE_MSAN "Enable the Memory Sanitizers" OFF)
if (LESTY_USE_MSAN)
    message("Enable Memory Sanitizer")
    target_compile_options(compiler_options INTERFACE
            -fsanitize=memory -fno-omit-frame-pointer)
    target_link_libraries(compiler_options INTERFACE
            -fsanitize=memory)
endif ()

option(LESTY_USE_UBSAN "Enable the Undefined Behavior Sanitizers" OFF)
if (LESTY_USE_UBSAN)
    message("Enable Undefined Behavior Sanitizer")
    target_compile_options(compiler_options INTERFACE
            -fsanitize=undefined)
    target_link_libraries(compiler_options INTERFACE
            -fsanitize=undefined)
endif ()