#pragma once

#include <cstddef>
#include <string>

#include "mosaic/color.h"

namespace mosaic {

class Image;
class Scene;
class SpriteSheet;

struct SceneReport {
  std::size_t width = 0;
  std::size_t height = 0;
  std::size_t layers = 0;
  std::size_t pixels = 0;
  std::size_t distinct_colors = 0;
  Color average = {};
  std::string preview;
};

SceneReport BuildSceneReport(const Scene& scene, const Image& image);
std::string FormatSceneReport(const SceneReport& report);
std::string DescribeSpriteSheet(const SpriteSheet& sheet);

}  // namespace mosaic