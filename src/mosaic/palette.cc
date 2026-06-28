#include "mosaic/palette.h"

#include "mosaic/scanner.h"

namespace mosaic {
namespace {

Color MakeFallbackColor() {
  return Color{};
}

}  // namespace

bool Palette::Parse(Scanner* scanner, std::string* error) {
  std::string mode;
  if (!scanner->NextToken(&mode)) {
    if (error) *error = "missing palette mode";
    return false;
  }
  if (mode == "flat") {
    gradient_ = false;
  } else if (mode == "gradient") {
    gradient_ = true;
  } else {
    if (error) *error = "unknown palette mode";
    return false;
  }

  std::size_t count = 0;
  if (!scanner->NextUnsigned(&count) || count == 0) {
    if (error) *error = "invalid palette size";
    return false;
  }

  colors_.clear();
  colors_.reserve(count);
  for (std::size_t i = 0; i < count; ++i) {
    Color color;
    if (!scanner->NextColor(&color)) {
      if (error) *error = "invalid palette color";
      return false;
    }
    colors_.push_back(color);
  }

  if (gradient_) {
    BuildGradient();
  }
  return true;
}

const Color& Palette::Lookup(std::size_t index) const {
  if (colors_.empty()) {
    static const Color kFallback = MakeFallbackColor();
    return kFallback;
  }
  return colors_[index % colors_.size()];
}

void Palette::BuildGradient() {
  std::vector<Color> expanded;
  expanded.reserve(colors_.size() * 2);
  for (std::size_t i = 0; i < colors_.size(); ++i) {
    expanded.push_back(colors_[i]);
    if (i + 1 < colors_.size()) {
      expanded.push_back(Blend(colors_[i], colors_[i + 1]));
    }
  }

  if (!colors_.empty() && (colors_.size() & 1U) != 0U) {
    expanded.push_back(colors_.back());
  }

  colors_.swap(expanded);
}

}  // namespace mosaic
