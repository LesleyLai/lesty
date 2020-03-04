add_executable(lesty-cli "main.cpp")
target_link_libraries(lesty-cli
        PRIVATE
        lesty::compiler_options
        lesty::lesty
        CONAN_PKG::nlohmann_json
        cxxopts::cxxopts
        )
add_clangformat(lesty-cli)

set_target_properties(lesty-cli PROPERTIES OUTPUT_NAME "lesty")