add_library(lesty
        include/aabb.hpp
        include/bounding_volume_hierarchy.hpp
        src/bounding_volume_hierarchy.cpp
        include/image.hpp
        src/image.cpp
        include/camera.hpp
        include/color.hpp
        include/hitable.hpp
        include/material.hpp
        src/material.cpp
        src/renderers/path_tracing_renderer.hpp
        src/renderers/path_tracing_renderer.cpp
        include/ray.hpp
        include/scene.hpp
        include/tile.hpp
        src/scene.cpp
        src/aabb.cpp
        src/hitables/axis_aligned_rect.hpp
        src/hitables/axis_aligned_rect.cpp
        src/hitables/sphere.hpp
        src/hitables/sphere.cpp
        src/hitables/triangle.hpp
        src/hitables/triangle.cpp
        include/scene_parser.hpp
        src/scene_parser.cpp
        src/renderer.cpp
        include/renderer.hpp)

target_link_libraries(lesty
        PUBLIC
        beyond::core
        CONAN_PKG::stb
        PRIVATE
        CONAN_PKG::CTRE
        CONAN_PKG::nlohmann_json
        lesty::compiler_options
        )
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
