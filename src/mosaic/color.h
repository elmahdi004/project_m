#pragma once

#include <cstdint>

namespace mosaic {

struct Color {
  uint8_t r = 0;
  uint8_t g = 0;
  uint8_t b = 0;
  uint8_t a = 255;
};

inline Color Blend(const Color& lhs, const Color& rhs) {
  Color blended;
  blended.r = static_cast<uint8_t>((static_cast<unsigned>(lhs.r) + rhs.r) / 2);
  blended.g = static_cast<uint8_t>((static_cast<unsigned>(lhs.g) + rhs.g) / 2);
  blended.b = static_cast<uint8_t>((static_cast<unsigned>(lhs.b) + rhs.b) / 2);
  blended.a = static_cast<uint8_t>((static_cast<unsigned>(lhs.a) + rhs.a) / 2);
  return blended;
}

}  // namespace mosaic
