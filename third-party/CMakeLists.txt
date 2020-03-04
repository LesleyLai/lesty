# beyond::core
add_subdirectory(beyond-core)

# indicators
add_library(indicators INTERFACE)
add_library(indicators::indicators ALIAS indicators)
target_include_directories(indicators
        SYSTEM INTERFACE
        ${CMAKE_CURRENT_SOURCE_DIR}/indicators/include
        )

# cxxopts
add_library(cxxopts INTERFACE)
add_library(cxxopts::cxxopts ALIAS cxxopts)
target_include_directories(cxxopts
        SYSTEM INTERFACE
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/cxxopts/include>
        $<INSTALL_INTERFACE:include>
        )