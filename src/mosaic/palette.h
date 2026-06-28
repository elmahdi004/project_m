#pragma once

#include <cstddef>
#include <string>
#include <vector>

#include "mosaic/color.h"

namespace mosaic {

class Scanner;

class Palette {
 public:
  bool Parse(Scanner* scanner, std::string* error);
  const Color& Lookup(std::size_t index) const;
  std::size_t size() const { return colors_.size(); }

 private:
  void BuildGradient();

  std::vector<Color> colors_;
  bool gradient_ = false;
};

}  // namespace mosaic
