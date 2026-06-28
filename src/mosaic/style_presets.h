#pragma once

#include <string>
#include <string_view>
#include <vector>

namespace mosaic {

class Image;

struct StylePreset {
  std::string_view name;
  std::string_view description;
  void (*apply)(Image* image);
};

const StylePreset* FindStylePreset(std::string_view name);
std::vector<std::string_view> ListStylePresetNames();
bool ApplyStylePreset(std::string_view name, Image* image);

}  // namespace mosaic
