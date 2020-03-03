add_library(lesty
        include/aabb.hpp
        include/axis_aligned_rect.hpp
        src/axis_aligned_rect.cpp
        include/bounding_volume_hierarchy.hpp
        src/bounding_volume_hierarchy.cpp
        include/image.hpp
        src/image.cpp
        include/camera.hpp
        include/color.hpp
        include/hitable.hpp
        include/material.hpp
        src/material.cpp
        include/pathtracer.hpp
        src/pathtracer.cpp
        include/ray.hpp
        include/sphere.hpp
        src/sphere.cpp
        include/scene.hpp
        include/tile.hpp
        src/scene.cpp src/aabb.cpp)

target_link_libraries(lesty
        PUBLIC
        beyond::core
        CONAN_PKG::stb
        indicators::indicators
        PRIVATE
        #lesty::compiler_options
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