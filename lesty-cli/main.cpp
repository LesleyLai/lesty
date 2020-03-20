#include <chrono>
#include <cstdio>
#include <iostream>
#include <vector>

#ifdef _MSC_VER
#pragma warning(push)
#pragma warning(disable : 4702)
#endif

#include <cxxopts.hpp>

#ifdef _MSC_VER
#pragma warning(pop)
#endif

#include <fmt/format.h>

#include "image.hpp"
#include "pathtracer.hpp"
#include "scene_parser.hpp"

#include <indicators/progress_bar.hpp>

using namespace lesty;

template <typename Duration>
[[nodiscard]] auto get_elapse_time(const Duration& elapsed_time) -> std::string
{
  using namespace std::chrono;

  if (elapsed_time < 1s) {
    return fmt::format("{}ms",
                       duration_cast<milliseconds>(elapsed_time).count());
  } else if (elapsed_time < 1min) {
    return fmt::format("{}s\n", duration_cast<seconds>(elapsed_time).count());
  } else {
    auto s = duration_cast<seconds>(elapsed_time).count();
    return fmt::format("{}min {}s", s / 60, s % 60);
  }
}

struct Options {
  std::size_t spp;
  std::size_t width;
  std::size_t height;
  std::string input_filename;
  std::string output_filename;
};

[[nodiscard]] auto parse_cmd(int argc, char** argv) -> Options
{
  cxxopts::Options options("lesty",
                           "Command line interface of the lesty renderer");

  options.positional_help("FILE").show_positional_help();

  // clang-format off
  options.add_options()
      ("h,help", "Print help")
      ("i,input_filename", "File name of the input_filename", cxxopts::value<std::string>());
  // clang-format on

  // clang-format off
  options.add_options("Renderer")
      ("spp","Samples per pixel, only useful for algorithms that support it",cxxopts::value<size_t>()->default_value("10"));
  // clang-format on

  // clang-format off
  options.add_options("Output")
      ("o,output", "File name of the output image", cxxopts::value<std::string>()->default_value("output.png"))
      ("width","Width of the output image in pixels",cxxopts::value<size_t>()->default_value("800"))
      ("height","Height of the output image in pixels",cxxopts::value<size_t>()->default_value("600"));
  // clang-format on

  options.parse_positional({"input_filename"});

  const auto print_help = [options]() {
    std::puts(options.help({"", "Renderer", "Output"}).c_str());
  };

  auto result = [&]() {
    try {
      return options.parse(argc, argv);
    } catch (cxxopts::OptionParseException& e) {
      fmt::print(stderr, "Error: {}\n", e.what());
      std::exit(-1);
    }
  }();

  if (result.count("help")) {
    std::puts(options.help({"", "Renderer"}).c_str());
    exit(0);
  }

  const auto input_filename = [&]() {
    if (result.count("input_filename")) {
      return result["input_filename"].as<std::string>();
    } else {
      std::fputs("Error: Need an input_filename file\n\n", stderr);
      std::exit(-1);
    }
  }();
  fmt::print("input_filename file: {}\n", input_filename);

  const auto spp = result["spp"].as<size_t>();
  const auto width = result["width"].as<size_t>();
  const auto height = result["height"].as<size_t>();
  const auto output_filename = result["output"].as<std::string>();

  fmt::print("width: {}, height: {}, sample size: {}\n", width, height, spp);

  return Options{.spp = spp,
                 .width = width,
                 .height = height,
                 .input_filename = input_filename,
                 .output_filename = output_filename};
}

int main(int argc, char** argv)
try {
  using namespace std::chrono;
  using namespace beyond::literals;

  const auto options = parse_cmd(argc, argv);

  std::ifstream input_file{options.input_filename};
  if (!input_file.is_open()) {
    fmt::print(stderr, "Error: cannot open file \"{}\"\n",
               options.input_filename);
    std::exit(2);
  }
  const auto scene = parse_scene(input_file);

  Path_tracer path_tracer;

  indicators::ProgressBar progress_bar{
      indicators::option::BarWidth{50},
      indicators::option::Start{"["},
      indicators::option::Fill{"="},
      indicators::option::Lead{">"},
      indicators::option::Remainder{" "},
      indicators::option::End{"]"},
      indicators::option::PostfixText{"Rendering"},
      indicators::option::ShowPercentage{true},
      indicators::option::ForegroundColor{indicators::Color::green}};
  path_tracer.set_progress_callback([&progress_bar](double progress) {
    progress_bar.set_progress(progress);
  });

  Image image(options.width, options.height);

  const auto aspect_ratio =
      static_cast<float>(options.width) / static_cast<float>(options.height);
  const Camera camera{
      {278, 278, -800}, {278, 278, 0}, {0, 1, 0}, 40.0_deg, aspect_ratio};

  const auto start = std::chrono::system_clock::now();
  path_tracer.run(scene, camera, image, options.spp);
  const auto end = std::chrono::system_clock::now();

  std::fflush(stdout);
  fmt::print("Elapsed time: {}\n", get_elapse_time(end - start));

  image.saveto(options.output_filename);
  fmt::print("Save image to {}\n", options.output_filename);
  return 0;
} catch (const std::exception& e) {
  fmt::print(stderr, "Error: {}\n", e.what());
  throw e;
} catch (...) {
  std::fputs("Unknown exception", stderr);
  return -255;
}