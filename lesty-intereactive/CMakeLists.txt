add_executable(lesty-intereactive "main.cpp")
target_link_libraries(lesty-intereactive
        PRIVATE
        lesty::compiler_options
        lesty::lesty
        indicators::indicators
        cxxopts::cxxopts
        CONAN_PKG::sdl2
        )
add_clangformat(lesty-intereactive)