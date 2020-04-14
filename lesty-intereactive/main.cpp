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
#include "renderer.hpp"
#include "scene_parser.hpp"

#include <SDL2/SDL.h>
#include <future>

using namespace lesty;

bool running = true;

auto handle_input() -> void
{
  SDL_Event sdl_event;
  while (SDL_PollEvent(&sdl_event) != 0) {
    switch (sdl_event.type) {
    case SDL_QUIT:
      running = false;
      break;
    default:
      return;
    }
  }
}

auto main() -> int
{
  SDL_Init(SDL_INIT_VIDEO); // Initialize SDL2

  const std::size_t width = 640;
  const std::size_t height = 480;

  // Create an application window with the following settings:
  auto* window = SDL_CreateWindow("lesty-intereactive",    // window title
                                  SDL_WINDOWPOS_UNDEFINED, // initial x position
                                  SDL_WINDOWPOS_UNDEFINED, // initial y position
                                  static_cast<int>(width), // width, in pixels
                                  static_cast<int>(height), // height, in pixels
                                  SDL_WINDOW_OPENGL         // flags - see below
  );
  // Check that the window was successfully created
  if (window == nullptr) {
    // In the case that the window could not be made...
    fmt::print("Could not create window: {}\n", SDL_GetError());
    return 1;
  }

  auto* renderer = SDL_CreateRenderer(window, 0, 0);

  auto* texture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_ARGB8888,
                                    SDL_TEXTUREACCESS_STATIC, width, height);

  std::unique_ptr<uint32_t[]> pixels{new Uint32[width * height]};
  memset(pixels.get(), 255, width * height * sizeof(uint32_t));

  std::ifstream input_file{"cornell.json"};
  const auto scene = lesty::parse_scene(input_file);

  const lesty::Options options{.spp = 100, .width = width, .height = height};
  const auto lesty_renderer =
      lesty::create_renderers(Renderer::Type::path, options);

  std::vector<std::future<void>> results;

  for (size_t start_y = 0; start_y < height; start_y += tile_size) {
    const size_t end_y = std::min(start_y + tile_size, height);
    for (size_t start_x = 0; start_x < width; start_x += tile_size) {
      const size_t end_x = std::min(start_x + tile_size, width);
      assert(start_x < end_x && start_y < end_y);
      TileDesc tile_desc{start_x, start_y, end_x - start_x, end_y - start_y};

      results.push_back(std::async(std::launch::async, [&lesty_renderer,
                                                        &pixels, tile_desc,
                                                        &scene] {
        for (std::uint32_t spp = 8; spp < 1000; spp <<= 2u) {
          auto tile = lesty_renderer->render_tile(tile_desc, scene, spp);

          for (size_t y = 0; y < tile.height(); ++y) {
            const auto image_y = tile.start_y() + y;
            for (size_t x = 0; x < tile.width(); ++x) {
              const auto image_x = tile.start_x() + x;
              Color color = tile.at(x, y);

              const auto int_r =
                  static_cast<uint32_t>(std::pow(color.r, 1 / 4.4f) * 255.99f);
              const auto int_g =
                  static_cast<uint32_t>(std::pow(color.g, 1 / 4.4f) * 255.99f);
              const auto int_b =
                  static_cast<uint32_t>(std::pow(color.b, 1 / 4.4f) * 255.99f);
              pixels[(height - 1 - image_y) * width + image_x] =
                  0xFF000000 | (int_r << 16u) | (int_g << 8u) | int_b;
            }
          }
        }
      }));
    }
  }

  while (running) {
    SDL_UpdateTexture(texture, nullptr, static_cast<void*>(pixels.get()),
                      width * sizeof(Uint32));

    handle_input();

    SDL_RenderClear(renderer);
    SDL_RenderCopy(renderer, texture, nullptr, nullptr);
    SDL_RenderPresent(renderer);
  }

  pixels.reset();

  SDL_DestroyTexture(texture);

  SDL_DestroyRenderer(renderer);

  // Close and destroy the window
  SDL_DestroyWindow(window);

  // Clean up
  SDL_Quit();
  return 0;
}