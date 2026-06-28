#pragma once

#include <cstddef>
#include <string>
#include <vector>

#include "mosaic/color.h"

namespace mosaic {

class Image;

enum class BlendMode {
  Copy,
  Multiply,
  Screen,
  Overlay,
  Darken,
  Lighten,
};

void Invert(Image* image);
void Grayscale(Image* image);
void Sepia(Image* image);
void AdjustBrightness(Image* image, int delta);
void AdjustContrast(Image* image, double factor);
void AdjustSaturation(Image* image, double factor);
void ApplyGamma(Image* image, double gamma);
void PremultiplyAlpha(Image* image);

Image Rotate90Clockwise(const Image& image);
Image Rotate180(const Image& image);
Image Rotate270Clockwise(const Image& image);
Image CropSafe(const Image& image, std::size_t x, std::size_t y, std::size_t width, std::size_t height);
Image Pad(const Image& image, std::size_t left, std::size_t top, std::size_t right, std::size_t bottom, Color fill);
Image BlendImages(const Image& base, const Image& overlay, BlendMode mode);
Image ResizeBilinear(const Image& image, std::size_t width, std::size_t height);
Image ApplyKernel(const Image& image, const std::vector<double>& kernel, std::size_t size, double divisor, double offset);
Image BoxBlur(const Image& image, std::size_t radius);
Image Sharpen(const Image& image);
Image EdgeDetect(const Image& image);
Image Emboss(const Image& image);
Image MedianFilter(const Image& image, std::size_t radius);
Image Dilate(const Image& image, std::size_t radius);
Image Erode(const Image& image, std::size_t radius);
Image HistogramEqualize(const Image& image);
Image Posterize(const Image& image, std::size_t levels);
Image Threshold(const Image& image, uint8_t cutoff);
Image Tint(const Image& image, Color tint, double amount);
Image AutoContrast(const Image& image);
Image CropOpaqueBounds(const Image& image);
Image QuantizeNearest(const Image& image, const std::vector<Color>& palette);
std::vector<Color> ExtractDominantColors(const Image& image, std::size_t max_colors);
Image FloydSteinbergDither(const Image& image, const std::vector<Color>& palette);
Image Checkerboard(std::size_t width, std::size_t height, Color a, Color b, std::size_t cell_size);

void DrawLine(Image* image, int x0, int y0, int x1, int y1, Color color);
void DrawRectangle(Image* image, int x, int y, int width, int height, Color color, bool filled);
void DrawCircle(Image* image, int cx, int cy, int radius, Color color, bool filled);
void FillTriangle(Image* image, int x0, int y0, int x1, int y1, int x2, int y2, Color color);

std::string SummarizeHistogram(const Image& image);
std::string DescribeColorBalance(const Image& image);

}  // namespace mosaic
