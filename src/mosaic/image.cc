#include "mosaic/image.h"

#include <algorithm>
#include <cmath>
#include <sstream>
#include <stdexcept>
#include <unordered_set>

namespace mosaic {
namespace {

Color BlendOver(const Color& background, const Color& source) {
  const unsigned source_alpha = source.a;
  const unsigned inv_alpha = 255U - source_alpha;
  Color result;
  result.r = static_cast<uint8_t>((source.r * source_alpha + background.r * inv_alpha) / 255U);
  result.g = static_cast<uint8_t>((source.g * source_alpha + background.g * inv_alpha) / 255U);
  result.b = static_cast<uint8_t>((source.b * source_alpha + background.b * inv_alpha) / 255U);
  result.a = static_cast<uint8_t>(std::min<unsigned>(255U, source_alpha + (background.a * inv_alpha) / 255U));
  return result;
}

char ShadeForColor(const Color& color) {
  const unsigned intensity = static_cast<unsigned>(color.r) + color.g + color.b;
  if (intensity < 120U) return ' ';
  if (intensity < 240U) return '.';
  if (intensity < 380U) return '*';
  if (intensity < 510U) return 'o';
  return '#';
}

}  // namespace

Image::Image() = default;

Image::Image(std::size_t width, std::size_t height, Color fill) {
  Resize(width, height, fill);
}

void Image::Resize(std::size_t width, std::size_t height, Color fill) {
  width_ = width;
  height_ = height;
  pixels_.assign(width_ * height_, fill);
}

void Image::Fill(Color color) {
  std::fill(pixels_.begin(), pixels_.end(), color);
}

std::size_t Image::Index(std::size_t x, std::size_t y) const {
  if (x >= width_ || y >= height_) {
    throw std::out_of_range("image coordinate out of range");
  }
  return y * width_ + x;
}

Color& Image::At(std::size_t x, std::size_t y) {
  return pixels_.at(Index(x, y));
}

const Color& Image::At(std::size_t x, std::size_t y) const {
  return pixels_.at(Index(x, y));
}

void Image::SetPixel(std::size_t x, std::size_t y, Color color) {
  At(x, y) = color;
}

void Image::BlendPixel(std::size_t x, std::size_t y, Color source) {
  At(x, y) = BlendOver(At(x, y), source);
}

void Image::FlipHorizontal() {
  for (std::size_t y = 0; y < height_; ++y) {
    for (std::size_t x = 0; x < width_ / 2; ++x) {
      std::swap(pixels_[Index(x, y)], pixels_[Index(width_ - 1 - x, y)]);
    }
  }
}

void Image::FlipVertical() {
  for (std::size_t y = 0; y < height_ / 2; ++y) {
    for (std::size_t x = 0; x < width_; ++x) {
      std::swap(pixels_[Index(x, y)], pixels_[Index(x, height_ - 1 - y)]);
    }
  }
}

Image Image::Crop(std::size_t x, std::size_t y, std::size_t width, std::size_t height) const {
  Image cropped(width, height, Color{});
  for (std::size_t row = 0; row < height; ++row) {
    for (std::size_t column = 0; column < width; ++column) {
      const std::size_t source_x = x + column;
      const std::size_t source_y = y + row;
      if (source_x < width_ && source_y < height_) {
        cropped.SetPixel(column, row, At(source_x, source_y));
      }
    }
  }
  return cropped;
}

Image Image::ResizedNearest(std::size_t width, std::size_t height) const {
  Image resized(width, height, Color{});
  if (width_ == 0 || height_ == 0 || width == 0 || height == 0) {
    return resized;
  }
  for (std::size_t y = 0; y < height; ++y) {
    const std::size_t source_y = std::min(height_ - 1, (y * height_) / height);
    for (std::size_t x = 0; x < width; ++x) {
      const std::size_t source_x = std::min(width_ - 1, (x * width_) / width);
      resized.SetPixel(x, y, At(source_x, source_y));
    }
  }
  return resized;
}

Color Image::AverageColor() const {
  if (pixels_.empty()) {
    return Color{};
  }
  unsigned long long r = 0;
  unsigned long long g = 0;
  unsigned long long b = 0;
  unsigned long long a = 0;
  for (const Color& color : pixels_) {
    r += color.r;
    g += color.g;
    b += color.b;
    a += color.a;
  }
  const unsigned long long count = pixels_.size();
  return Color{
      static_cast<uint8_t>(r / count),
      static_cast<uint8_t>(g / count),
      static_cast<uint8_t>(b / count),
      static_cast<uint8_t>(a / count),
  };
}

std::size_t Image::CountDistinctColors() const {
  struct Hasher {
    std::size_t operator()(const Color& color) const {
      return (static_cast<std::size_t>(color.r) << 24) ^ (static_cast<std::size_t>(color.g) << 16) ^
             (static_cast<std::size_t>(color.b) << 8) ^ static_cast<std::size_t>(color.a);
    }
  };

  struct Equality {
    bool operator()(const Color& lhs, const Color& rhs) const {
      return lhs.r == rhs.r && lhs.g == rhs.g && lhs.b == rhs.b && lhs.a == rhs.a;
    }
  };

  std::unordered_set<Color, Hasher, Equality> unique_colors;
  unique_colors.reserve(pixels_.size());
  for (const Color& color : pixels_) {
    unique_colors.insert(color);
  }
  return unique_colors.size();
}

std::string Image::AsciiPreview(std::size_t max_width) const {
  if (width_ == 0 || height_ == 0) {
    return "[empty image]\n";
  }
  const std::size_t preview_width = std::max<std::size_t>(1, std::min(width_, max_width));
  const std::size_t preview_height = std::max<std::size_t>(1, (preview_width * height_ + width_ - 1) / width_);

  Image sampled = ResizedNearest(preview_width, preview_height);
  std::ostringstream out;
  for (std::size_t y = 0; y < sampled.height(); ++y) {
    for (std::size_t x = 0; x < sampled.width(); ++x) {
      out << ShadeForColor(sampled.At(x, y));
    }
    out << '\n';
  }
  return out.str();
}

void Image::ReplacePixels(std::size_t width, std::size_t height, std::vector<Color> pixels) {
  if (pixels.size() != width * height) {
    throw std::invalid_argument("pixel buffer dimensions do not match");
  }
  width_ = width;
  height_ = height;
  pixels_ = std::move(pixels);
}

}  // namespace mosaic