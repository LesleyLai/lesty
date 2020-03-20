#pragma once

#ifndef LESTY_SCENE_PARSER_HPP
#define LESTY_SCENE_PARSER_HPP

#include <fstream>

#include "scene.hpp"

namespace lesty {

[[nodiscard]] auto parse_scene(std::ifstream& file) -> Scene;

}

#endif // LESTY_SCENE_PARSER_HPP
