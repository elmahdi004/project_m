#pragma once

#include <cstddef>
#include <string>
#include <vector>

#include "mosaic/color.h"

namespace mosaic {

class Palette;
class RleDecoder;
class Scanner;

struct Layer {
  std::string name;
  bool visible = true;
  std::vector<Color> pixels;
};

class Scene {
 public:
  bool Parse(Scanner* scanner, std::string* error);
  std::vector<Color> Composite() const;
  std::size_t layer_count() const { return layers_.size(); }
  std::size_t width() const { return width_; }
  std::size_t height() const { return height_; }
  const Palette& palette() const { return palette_; }

 private:
  std::size_t width_ = 0;
  std::size_t height_ = 0;
  Palette palette_;
  std::vector<Layer> layers_;
  std::vector<Color> background_;
};

}  // namespace mosaic
