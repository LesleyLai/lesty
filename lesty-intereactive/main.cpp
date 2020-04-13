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

auto main(int argc, char** argv) -> int
{
  SDL_Init(SDL_INIT_VIDEO); // Initialize SDL2

  const int width = 640;
  const int height = 480;

  // Create an application window with the following settings:
  auto* window = SDL_CreateWindow("lesty-intereactive",    // window title
                                  SDL_WINDOWPOS_UNDEFINED, // initial x position
                                  SDL_WINDOWPOS_UNDEFINED, // initial y position
                                  width,                   // width, in pixels
                                  height,                  // height, in pixels
                                  SDL_WINDOW_OPENGL        // flags - see below
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

  auto* pixels = new Uint32[width * height];
  memset(pixels, 255, width * height * sizeof(Uint32));

  std::ifstream input_file{"cornell.json"};
  const auto scene = lesty::parse_scene(input_file);

  const lesty::Options options{.spp = 100, .width = width, .height = height};
  const auto lesty_renderer =
      lesty::create_renderers(Renderer::Type::path, options);
  const auto image_out = lesty_renderer->render(scene);

  for (auto y = 0; y < height; ++y) {
    for (auto x = 0; x < width; ++x) {
      Color color = image_out.color_at(x, (height - y - 1));
      const auto int_r = static_cast<uint32_t>(pow(color.r, 1 / 2.2) * 255.99f);
      const auto int_g = static_cast<uint32_t>(pow(color.g, 1 / 2.2) * 255.99f);
      const auto int_b = static_cast<uint32_t>(pow(color.b, 1 / 2.2) * 255.99f);
      pixels[y * width + x] =
          0xFF000000 | (int_r << 16u) | (int_g << 8u) | int_b;
    }
  }

  while (running) {
    SDL_UpdateTexture(texture, nullptr, static_cast<void*>(pixels),
                      width * sizeof(Uint32));

    handle_input();

    SDL_RenderClear(renderer);
    SDL_RenderCopy(renderer, texture, nullptr, nullptr);
    SDL_RenderPresent(renderer);
  }

  SDL_DestroyTexture(texture);

  SDL_DestroyRenderer(renderer);

  // Close and destroy the window
  SDL_DestroyWindow(window);

  // Clean up
  SDL_Quit();
  return 0;
}