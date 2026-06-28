#pragma once

#include <cstddef>
#include <string>
#include <vector>

#include "mosaic/color.h"

namespace mosaic {

class Image;

struct MontageLayout {
  std::size_t columns = 0;
  std::size_t padding = 0;
  Color background = {24, 28, 36, 255};
  Color border = {90, 96, 110, 255};
};

Image BuildMontage(const std::vector<Image>& images, const MontageLayout& layout);
Image BuildMontageFromDirectory(const std::string& directory, std::string* error, const MontageLayout& layout = MontageLayout{});

}  // namespace mosaic
