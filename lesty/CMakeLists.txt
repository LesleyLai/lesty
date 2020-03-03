add_library(lesty
        src/dummy.cpp)
target_link_libraries(lesty
        PUBLIC beyond::core
        PRIVATE lesty::compiler_options)
target_include_directories(lesty PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/include")
add_clangformat(lesty)
add_library(lesty::lesty ALIAS lesty)

if (CMAKE_PROJECT_NAME STREQUAL PROJECT_NAME AND BUILD_TESTING)
    if (${LESTY_BUILD_TESTS_COVERAGE})
        target_compile_options(lesty PUBLIC -fprofile-arcs -ftest-coverage)
        target_link_libraries(lesty PUBLIC gcov)
    endif ()

    add_subdirectory(test)
endif ()