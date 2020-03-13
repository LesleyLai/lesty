cmake_minimum_required(VERSION 3.14)

project(lesty VERSION 0.0.2 LANGUAGES CXX)

include("cmake/StandardProjectSettings.cmake")
include("CMakeDependentOption")

include("cmake/compiler.cmake")
include("cmake/clangformat.cmake")
include("cmake/StaticAnalyzers.cmake")

include("cmake/conan.cmake")
run_conan()

add_subdirectory(third-party)

if (CMAKE_PROJECT_NAME STREQUAL PROJECT_NAME)
    include(CTest)
endif ()

add_subdirectory(lesty)
add_subdirectory(lesty-cli)

# Copy assets
add_custom_target(lesty_scenes
        COMMAND ${CMAKE_COMMAND} -E copy_directory
        ${CMAKE_CURRENT_SOURCE_DIR}/scenes ${CMAKE_BINARY_DIR}/bin
        )

add_dependencies(lesty lesty_scenes)
