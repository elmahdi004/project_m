#pragma once

#include <cstddef>
#include <string>
#include <vector>

#include "mosaic/color.h"

namespace mosaic {

class Image {
 public:
  Image();
  Image(std::size_t width, std::size_t height, Color fill = Color{});

  std::size_t width() const { return width_; }
  std::size_t height() const { return height_; }
  bool empty() const { return pixels_.empty(); }

  void Resize(std::size_t width, std::size_t height, Color fill = Color{});
  void Fill(Color color);

  Color& At(std::size_t x, std::size_t y);
  const Color& At(std::size_t x, std::size_t y) const;

  void SetPixel(std::size_t x, std::size_t y, Color color);
  void BlendPixel(std::size_t x, std::size_t y, Color source);

  void FlipHorizontal();
  void FlipVertical();
  Image Crop(std::size_t x, std::size_t y, std::size_t width, std::size_t height) const;
  Image ResizedNearest(std::size_t width, std::size_t height) const;

  Color AverageColor() const;
  std::size_t CountDistinctColors() const;
  std::string AsciiPreview(std::size_t max_width = 72) const;

  const std::vector<Color>& pixels() const { return pixels_; }
  std::vector<Color>& mutable_pixels() { return pixels_; }
  void ReplacePixels(std::size_t width, std::size_t height, std::vector<Color> pixels);

 private:
  std::size_t Index(std::size_t x, std::size_t y) const;

  std::size_t width_ = 0;
  std::size_t height_ = 0;
  std::vector<Color> pixels_;
};

}  // namespace mosaic