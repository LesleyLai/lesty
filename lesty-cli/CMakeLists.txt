add_executable(lesty-cli "main.cpp")
target_link_libraries(lesty-cli
        PRIVATE
        lesty::lesty
        #lesty::compiler_options
        CONAN_PKG::nlohmann_json
        CONAN_PKG::docopt.cpp
        )
add_clangformat(lesty-cli)

set_target_properties(lesty-cli PROPERTIES OUTPUT_NAME "lesty")