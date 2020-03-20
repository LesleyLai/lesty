add_executable(lesty-cli "main.cpp")
target_link_libraries(lesty-cli
        PRIVATE
        lesty::compiler_options
        lesty::lesty
        indicators::indicators
        cxxopts::cxxopts
        )
add_clangformat(lesty-cli)

set_target_properties(lesty-cli PROPERTIES OUTPUT_NAME "lesty")