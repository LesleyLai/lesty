add_executable(lesty-cli "main.cpp")
target_link_libraries(lesty-cli
        PRIVATE lesty::lesty lesty::compiler_options)
add_clangformat(lesty-cli)

set_target_properties(lesty-cli PROPERTIES OUTPUT_NAME "lesty")