#pragma once

#include <cstddef>
#include <string>
#include <vector>

#include "mosaic/color.h"

namespace mosaic {

bool LookupPresetPalette(const std::string& name, std::vector<Color>* colors);
std::size_t PresetPaletteCount();

}  // namespace mosaic