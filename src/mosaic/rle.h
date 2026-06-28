#pragma once

#include <cstddef>
#include <string>
#include <vector>

#include "mosaic/color.h"

namespace mosaic {

class Palette;
class Scanner;

class RleDecoder {
 public:
  bool Parse(Scanner* scanner, std::string* error);
  std::vector<Color> DecodeRow(const Palette& palette, std::size_t width) const;
  std::vector<Color> DecodeImage(const Palette& palette) const;

 private:
  struct Span {
    std::size_t count = 0;
    std::size_t palette_index = 0;
    bool literal = false;
    Color literal_color;
  };

  std::vector<Span> spans_;
  std::size_t height_ = 0;
};

}  // namespace mosaic
