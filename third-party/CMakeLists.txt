# beyond::core
add_subdirectory(beyond-core)

# indicators
add_library(indicators INTERFACE)
add_library(indicators::indicators ALIAS indicators)
target_include_directories(indicators
        SYSTEM INTERFACE
        ${CMAKE_CURRENT_SOURCE_DIR}/indicators/include
        )