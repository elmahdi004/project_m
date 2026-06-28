#include "mosaic/image_ops.h"

#include <algorithm>
#include <array>
#include <cmath>
#include <cstdint>
#include <iomanip>
#include <functional>
#include <numeric>
#include <unordered_map>
#include <sstream>
#include <stdexcept>
#include <utility>
#include <vector>

#include "mosaic/image.h"

namespace mosaic {
namespace {

int ClampInt(int value, int low, int high) {
  return std::max(low, std::min(high, value));
}

double ClampDouble(double value, double low, double high) {
  return std::max(low, std::min(high, value));
}

uint8_t ClampToByte(double value) {
  return static_cast<uint8_t>(ClampInt(static_cast<int>(std::round(value)), 0, 255));
}

Color MakeColor(int r, int g, int b, int a = 255) {
  return Color{ClampToByte(r), ClampToByte(g), ClampToByte(b), ClampToByte(a)};
}

Color BlendModePixel(Color base, Color overlay, BlendMode mode) {
  auto channel = [](int lhs, int rhs, BlendMode blend_mode) -> int {
    switch (blend_mode) {
      case BlendMode::Copy:
        return rhs;
      case BlendMode::Multiply:
        return (lhs * rhs) / 255;
      case BlendMode::Screen:
        return 255 - ((255 - lhs) * (255 - rhs)) / 255;
      case BlendMode::Overlay:
        return lhs < 128 ? (2 * lhs * rhs) / 255 : 255 - (2 * (255 - lhs) * (255 - rhs)) / 255;
      case BlendMode::Darken:
        return std::min(lhs, rhs);
      case BlendMode::Lighten:
        return std::max(lhs, rhs);
    }
    return rhs;
  };

  Color out;
  out.r = static_cast<uint8_t>(channel(base.r, overlay.r, mode));
  out.g = static_cast<uint8_t>(channel(base.g, overlay.g, mode));
  out.b = static_cast<uint8_t>(channel(base.b, overlay.b, mode));
  out.a = static_cast<uint8_t>(std::max(base.a, overlay.a));
  return out;
}

Color LerpColor(const Color& lhs, const Color& rhs, double amount) {
  amount = ClampDouble(amount, 0.0, 1.0);
  const double inverse = 1.0 - amount;
  return Color{
      ClampToByte(lhs.r * inverse + rhs.r * amount),
      ClampToByte(lhs.g * inverse + rhs.g * amount),
      ClampToByte(lhs.b * inverse + rhs.b * amount),
      ClampToByte(lhs.a * inverse + rhs.a * amount),
  };
}

Color ToGray(const Color& color) {
  const int luminance = static_cast<int>(0.2126 * color.r + 0.7152 * color.g + 0.0722 * color.b);
  return MakeColor(luminance, luminance, luminance, color.a);
}

Color ToSepia(const Color& color) {
  const int tr = static_cast<int>(0.393 * color.r + 0.769 * color.g + 0.189 * color.b);
  const int tg = static_cast<int>(0.349 * color.r + 0.686 * color.g + 0.168 * color.b);
  const int tb = static_cast<int>(0.272 * color.r + 0.534 * color.g + 0.131 * color.b);
  return MakeColor(tr, tg, tb, color.a);
}

double SaturationFor(const Color& color) {
  const double max_channel = std::max({static_cast<double>(color.r), static_cast<double>(color.g), static_cast<double>(color.b)});
  const double min_channel = std::min({static_cast<double>(color.r), static_cast<double>(color.g), static_cast<double>(color.b)});
  if (max_channel <= 0.0) {
    return 0.0;
  }
  return (max_channel - min_channel) / max_channel;
}

Color AdjustSaturationColor(const Color& color, double factor) {
  const double gray = static_cast<double>(ToGray(color).r);
  const auto mix = [&](uint8_t channel) {
    return ClampToByte(gray + (static_cast<double>(channel) - gray) * factor);
  };
  return Color{mix(color.r), mix(color.g), mix(color.b), color.a};
}

void ApplyToAll(Image* image, const std::function<Color(Color)>& transform) {
  for (std::size_t y = 0; y < image->height(); ++y) {
    for (std::size_t x = 0; x < image->width(); ++x) {
      image->SetPixel(x, y, transform(image->At(x, y)));
    }
  }
}

Image MakeBlankLike(const Image& image) {
  return Image(image.width(), image.height(), Color{});
}

void DrawPixelSafe(Image* image, int x, int y, Color color) {
  if (x < 0 || y < 0) {
    return;
  }
  const std::size_t ux = static_cast<std::size_t>(x);
  const std::size_t uy = static_cast<std::size_t>(y);
  if (ux >= image->width() || uy >= image->height()) {
    return;
  }
  image->BlendPixel(ux, uy, color);
}

}  // namespace

void Invert(Image* image) {
  ApplyToAll(image, [](Color color) { return Color{static_cast<uint8_t>(255 - color.r), static_cast<uint8_t>(255 - color.g), static_cast<uint8_t>(255 - color.b), color.a}; });
}

void Grayscale(Image* image) {
  ApplyToAll(image, [](Color color) { return ToGray(color); });
}

void Sepia(Image* image) {
  ApplyToAll(image, [](Color color) { return ToSepia(color); });
}

void AdjustBrightness(Image* image, int delta) {
  ApplyToAll(image, [delta](Color color) {
    return MakeColor(static_cast<int>(color.r) + delta, static_cast<int>(color.g) + delta, static_cast<int>(color.b) + delta, color.a);
  });
}

void AdjustContrast(Image* image, double factor) {
  factor = ClampDouble(factor, 0.0, 4.0);
  ApplyToAll(image, [factor](Color color) {
    auto adjust = [factor](uint8_t channel) {
      const double centered = static_cast<double>(channel) - 128.0;
      return ClampToByte(128.0 + centered * factor);
    };
    return Color{adjust(color.r), adjust(color.g), adjust(color.b), color.a};
  });
}

void AdjustSaturation(Image* image, double factor) {
  factor = ClampDouble(factor, 0.0, 4.0);
  ApplyToAll(image, [factor](Color color) { return AdjustSaturationColor(color, factor); });
}

void ApplyGamma(Image* image, double gamma) {
  gamma = ClampDouble(gamma, 0.1, 10.0);
  const double inverse = 1.0 / gamma;
  ApplyToAll(image, [inverse](Color color) {
    auto apply = [inverse](uint8_t channel) {
      const double normalized = static_cast<double>(channel) / 255.0;
      return ClampToByte(std::pow(normalized, inverse) * 255.0);
    };
    return Color{apply(color.r), apply(color.g), apply(color.b), color.a};
  });
}

void PremultiplyAlpha(Image* image) {
  ApplyToAll(image, [](Color color) {
    const double alpha = static_cast<double>(color.a) / 255.0;
    return Color{
        ClampToByte(color.r * alpha),
        ClampToByte(color.g * alpha),
        ClampToByte(color.b * alpha),
        color.a,
    };
  });
}

Image Rotate90Clockwise(const Image& image) {
  Image rotated(image.height(), image.width(), Color{});
  for (std::size_t y = 0; y < image.height(); ++y) {
    for (std::size_t x = 0; x < image.width(); ++x) {
      rotated.SetPixel(image.height() - 1 - y, x, image.At(x, y));
    }
  }
  return rotated;
}

Image Rotate180(const Image& image) {
  Image rotated(image.width(), image.height(), Color{});
  for (std::size_t y = 0; y < image.height(); ++y) {
    for (std::size_t x = 0; x < image.width(); ++x) {
      rotated.SetPixel(image.width() - 1 - x, image.height() - 1 - y, image.At(x, y));
    }
  }
  return rotated;
}

Image Rotate270Clockwise(const Image& image) {
  Image rotated(image.height(), image.width(), Color{});
  for (std::size_t y = 0; y < image.height(); ++y) {
    for (std::size_t x = 0; x < image.width(); ++x) {
      rotated.SetPixel(y, image.width() - 1 - x, image.At(x, y));
    }
  }
  return rotated;
}

Image CropSafe(const Image& image, std::size_t x, std::size_t y, std::size_t width, std::size_t height) {
  return image.Crop(x, y, width, height);
}

Image Pad(const Image& image, std::size_t left, std::size_t top, std::size_t right, std::size_t bottom, Color fill) {
  Image padded(image.width() + left + right, image.height() + top + bottom, fill);
  for (std::size_t y = 0; y < image.height(); ++y) {
    for (std::size_t x = 0; x < image.width(); ++x) {
      padded.SetPixel(left + x, top + y, image.At(x, y));
    }
  }
  return padded;
}

Image BlendImages(const Image& base, const Image& overlay, BlendMode mode) {
  Image result(std::max(base.width(), overlay.width()), std::max(base.height(), overlay.height()), Color{});
  for (std::size_t y = 0; y < result.height(); ++y) {
    for (std::size_t x = 0; x < result.width(); ++x) {
      const bool has_base = x < base.width() && y < base.height();
      const bool has_overlay = x < overlay.width() && y < overlay.height();
      if (has_base && has_overlay) {
        result.SetPixel(x, y, BlendModePixel(base.At(x, y), overlay.At(x, y), mode));
      } else if (has_base) {
        result.SetPixel(x, y, base.At(x, y));
      } else if (has_overlay) {
        result.SetPixel(x, y, overlay.At(x, y));
      }
    }
  }
  return result;
}

Image ResizeBilinear(const Image& image, std::size_t width, std::size_t height) {
  Image resized(width, height, Color{});
  if (image.width() == 0 || image.height() == 0 || width == 0 || height == 0) {
    return resized;
  }
  for (std::size_t y = 0; y < height; ++y) {
    const double gy = (height == 1) ? 0.0 : static_cast<double>(y) * (image.height() - 1) / (height - 1);
    const std::size_t y0 = static_cast<std::size_t>(gy);
    const std::size_t y1 = std::min(image.height() - 1, y0 + 1);
    const double fy = gy - y0;
    for (std::size_t x = 0; x < width; ++x) {
      const double gx = (width == 1) ? 0.0 : static_cast<double>(x) * (image.width() - 1) / (width - 1);
      const std::size_t x0 = static_cast<std::size_t>(gx);
      const std::size_t x1 = std::min(image.width() - 1, x0 + 1);
      const double fx = gx - x0;

      const Color top = LerpColor(image.At(x0, y0), image.At(x1, y0), fx);
      const Color bottom = LerpColor(image.At(x0, y1), image.At(x1, y1), fx);
      resized.SetPixel(x, y, LerpColor(top, bottom, fy));
    }
  }
  return resized;
}

Image ApplyKernel(const Image& image, const std::vector<double>& kernel, std::size_t size, double divisor, double offset) {
  if (size == 0 || kernel.size() != size * size) {
    return image;
  }
  if (divisor == 0.0) {
    divisor = std::accumulate(kernel.begin(), kernel.end(), 0.0);
    if (divisor == 0.0) {
      divisor = 1.0;
    }
  }

  Image output(image.width(), image.height(), Color{});
  const int radius = static_cast<int>(size / 2);
  for (std::size_t y = 0; y < image.height(); ++y) {
    for (std::size_t x = 0; x < image.width(); ++x) {
      double r = 0.0;
      double g = 0.0;
      double b = 0.0;
      double a = 0.0;
      for (std::size_t ky = 0; ky < size; ++ky) {
        for (std::size_t kx = 0; kx < size; ++kx) {
          const int source_x = ClampInt(static_cast<int>(x) + static_cast<int>(kx) - radius, 0, static_cast<int>(image.width()) - 1);
          const int source_y = ClampInt(static_cast<int>(y) + static_cast<int>(ky) - radius, 0, static_cast<int>(image.height()) - 1);
          const double weight = kernel[ky * size + kx];
          const Color source = image.At(static_cast<std::size_t>(source_x), static_cast<std::size_t>(source_y));
          r += source.r * weight;
          g += source.g * weight;
          b += source.b * weight;
          a += source.a * weight;
        }
      }
      output.SetPixel(x, y, Color{
          ClampToByte(r / divisor + offset),
          ClampToByte(g / divisor + offset),
          ClampToByte(b / divisor + offset),
          ClampToByte(a / divisor + offset),
      });
    }
  }
  return output;
}

Image BoxBlur(const Image& image, std::size_t radius) {
  const std::size_t size = radius * 2 + 1;
  std::vector<double> kernel(size * size, 1.0);
  return ApplyKernel(image, kernel, size, 0.0, 0.0);
}

Image Sharpen(const Image& image) {
  return ApplyKernel(image, {
      0, -1, 0,
      -1, 5, -1,
      0, -1, 0,
  }, 3, 1.0, 0.0);
}

Image EdgeDetect(const Image& image) {
  return ApplyKernel(image, {
      -1, -1, -1,
      -1, 8, -1,
      -1, -1, -1,
  }, 3, 1.0, 128.0);
}

Image Emboss(const Image& image) {
  return ApplyKernel(image, {
      -2, -1, 0,
      -1, 1, 1,
      0, 1, 2,
  }, 3, 1.0, 128.0);
}

void DrawLine(Image* image, int x0, int y0, int x1, int y1, Color color) {
  const int dx = std::abs(x1 - x0);
  const int sx = x0 < x1 ? 1 : -1;
  const int dy = -std::abs(y1 - y0);
  const int sy = y0 < y1 ? 1 : -1;
  int error = dx + dy;

  while (true) {
    DrawPixelSafe(image, x0, y0, color);
    if (x0 == x1 && y0 == y1) {
      break;
    }
    const int doubled = 2 * error;
    if (doubled >= dy) {
      error += dy;
      x0 += sx;
    }
    if (doubled <= dx) {
      error += dx;
      y0 += sy;
    }
  }
}

void DrawRectangle(Image* image, int x, int y, int width, int height, Color color, bool filled) {
  if (width <= 0 || height <= 0) {
    return;
  }
  if (filled) {
    for (int row = 0; row < height; ++row) {
      for (int column = 0; column < width; ++column) {
        DrawPixelSafe(image, x + column, y + row, color);
      }
    }
    return;
  }

  for (int column = 0; column < width; ++column) {
    DrawPixelSafe(image, x + column, y, color);
    DrawPixelSafe(image, x + column, y + height - 1, color);
  }
  for (int row = 0; row < height; ++row) {
    DrawPixelSafe(image, x, y + row, color);
    DrawPixelSafe(image, x + width - 1, y + row, color);
  }
}

void DrawCircle(Image* image, int cx, int cy, int radius, Color color, bool filled) {
  if (radius <= 0) {
    return;
  }
  int x = radius;
  int y = 0;
  int error = 1 - x;
  while (x >= y) {
    if (filled) {
      DrawLine(image, cx - x, cy + y, cx + x, cy + y, color);
      DrawLine(image, cx - y, cy + x, cx + y, cy + x, color);
      DrawLine(image, cx - x, cy - y, cx + x, cy - y, color);
      DrawLine(image, cx - y, cy - x, cx + y, cy - x, color);
    } else {
      DrawPixelSafe(image, cx + x, cy + y, color);
      DrawPixelSafe(image, cx + y, cy + x, color);
      DrawPixelSafe(image, cx - y, cy + x, color);
      DrawPixelSafe(image, cx - x, cy + y, color);
      DrawPixelSafe(image, cx - x, cy - y, color);
      DrawPixelSafe(image, cx - y, cy - x, color);
      DrawPixelSafe(image, cx + y, cy - x, color);
      DrawPixelSafe(image, cx + x, cy - y, color);
    }
    ++y;
    if (error < 0) {
      error += 2 * y + 1;
    } else {
      --x;
      error += 2 * (y - x) + 1;
    }
  }
}

void FillTriangle(Image* image, int x0, int y0, int x1, int y1, int x2, int y2, Color color) {
  auto edge = [](int ax, int ay, int bx, int by, int cx, int cy) {
    return (cx - ax) * (by - ay) - (cy - ay) * (bx - ax);
  };

  const int min_x = std::min({x0, x1, x2});
  const int max_x = std::max({x0, x1, x2});
  const int min_y = std::min({y0, y1, y2});
  const int max_y = std::max({y0, y1, y2});

  for (int y = min_y; y <= max_y; ++y) {
    for (int x = min_x; x <= max_x; ++x) {
      const int w0 = edge(x1, y1, x2, y2, x, y);
      const int w1 = edge(x2, y2, x0, y0, x, y);
      const int w2 = edge(x0, y0, x1, y1, x, y);
      if ((w0 >= 0 && w1 >= 0 && w2 >= 0) || (w0 <= 0 && w1 <= 0 && w2 <= 0)) {
        DrawPixelSafe(image, x, y, color);
      }
    }
  }
}

std::string SummarizeHistogram(const Image& image) {
  std::array<std::size_t, 16> buckets{};
  for (const Color& color : image.pixels()) {
    const std::size_t luminance = static_cast<std::size_t>((static_cast<unsigned>(color.r) + color.g + color.b) / 3);
    const std::size_t bucket = std::min<std::size_t>(15, luminance / 16);
    ++buckets[bucket];
  }
  std::ostringstream out;
  out << "histogram";
  for (std::size_t i = 0; i < buckets.size(); ++i) {
    out << ' ' << i << ':' << buckets[i];
  }
  return out.str();
}

std::string DescribeColorBalance(const Image& image) {
  if (image.empty()) {
    return "balance empty";
  }
  unsigned long long red = 0;
  unsigned long long green = 0;
  unsigned long long blue = 0;
  for (const Color& color : image.pixels()) {
    red += color.r;
    green += color.g;
    blue += color.b;
  }
  const unsigned long long total = static_cast<unsigned long long>(image.pixels().size());
  std::ostringstream out;
  out << "balance r=" << (red / total) << " g=" << (green / total) << " b=" << (blue / total);
  return out.str();
}

Image MedianFilter(const Image& image, std::size_t radius) {
  if (image.empty()) {
    return image;
  }
  Image output(image.width(), image.height(), Color{});
  const int r = static_cast<int>(radius);
  std::vector<uint8_t> red;
  std::vector<uint8_t> green;
  std::vector<uint8_t> blue;
  std::vector<uint8_t> alpha;
  red.reserve((radius * 2 + 1) * (radius * 2 + 1));
  green.reserve(red.capacity());
  blue.reserve(red.capacity());
  alpha.reserve(red.capacity());

  for (std::size_t y = 0; y < image.height(); ++y) {
    for (std::size_t x = 0; x < image.width(); ++x) {
      red.clear();
      green.clear();
      blue.clear();
      alpha.clear();
      for (int dy = -r; dy <= r; ++dy) {
        for (int dx = -r; dx <= r; ++dx) {
          const int sx = ClampInt(static_cast<int>(x) + dx, 0, static_cast<int>(image.width()) - 1);
          const int sy = ClampInt(static_cast<int>(y) + dy, 0, static_cast<int>(image.height()) - 1);
          const Color sample = image.At(static_cast<std::size_t>(sx), static_cast<std::size_t>(sy));
          red.push_back(sample.r);
          green.push_back(sample.g);
          blue.push_back(sample.b);
          alpha.push_back(sample.a);
        }
      }
      auto middle = [&](std::vector<uint8_t>& values) {
        std::nth_element(values.begin(), values.begin() + values.size() / 2, values.end());
        return values[values.size() / 2];
      };
      output.SetPixel(x, y, Color{middle(red), middle(green), middle(blue), middle(alpha)});
    }
  }
  return output;
}

Image Dilate(const Image& image, std::size_t radius) {
  if (image.empty()) {
    return image;
  }
  Image output(image.width(), image.height(), Color{});
  const int r = static_cast<int>(radius);
  for (std::size_t y = 0; y < image.height(); ++y) {
    for (std::size_t x = 0; x < image.width(); ++x) {
      Color best = image.At(x, y);
      unsigned best_score = static_cast<unsigned>(best.r) + best.g + best.b;
      for (int dy = -r; dy <= r; ++dy) {
        for (int dx = -r; dx <= r; ++dx) {
          const int sx = ClampInt(static_cast<int>(x) + dx, 0, static_cast<int>(image.width()) - 1);
          const int sy = ClampInt(static_cast<int>(y) + dy, 0, static_cast<int>(image.height()) - 1);
          const Color sample = image.At(static_cast<std::size_t>(sx), static_cast<std::size_t>(sy));
          const unsigned score = static_cast<unsigned>(sample.r) + sample.g + sample.b;
          if (score > best_score) {
            best_score = score;
            best = sample;
          }
        }
      }
      output.SetPixel(x, y, best);
    }
  }
  return output;
}

Image Erode(const Image& image, std::size_t radius) {
  if (image.empty()) {
    return image;
  }
  Image output(image.width(), image.height(), Color{});
  const int r = static_cast<int>(radius);
  for (std::size_t y = 0; y < image.height(); ++y) {
    for (std::size_t x = 0; x < image.width(); ++x) {
      Color best = image.At(x, y);
      unsigned best_score = static_cast<unsigned>(best.r) + best.g + best.b;
      for (int dy = -r; dy <= r; ++dy) {
        for (int dx = -r; dx <= r; ++dx) {
          const int sx = ClampInt(static_cast<int>(x) + dx, 0, static_cast<int>(image.width()) - 1);
          const int sy = ClampInt(static_cast<int>(y) + dy, 0, static_cast<int>(image.height()) - 1);
          const Color sample = image.At(static_cast<std::size_t>(sx), static_cast<std::size_t>(sy));
          const unsigned score = static_cast<unsigned>(sample.r) + sample.g + sample.b;
          if (score < best_score) {
            best_score = score;
            best = sample;
          }
        }
      }
      output.SetPixel(x, y, best);
    }
  }
  return output;
}

Image HistogramEqualize(const Image& image) {
  if (image.empty()) {
    return image;
  }
  std::array<std::size_t, 256> histogram{};
  for (const Color& color : image.pixels()) {
    const std::size_t luminance = static_cast<std::size_t>((static_cast<unsigned>(color.r) + color.g + color.b) / 3);
    ++histogram[luminance];
  }

  std::array<std::size_t, 256> cdf{};
  std::partial_sum(histogram.begin(), histogram.end(), cdf.begin());
  const std::size_t total = image.pixels().size();
  const std::size_t cdf_min = *std::find_if(cdf.begin(), cdf.end(), [](std::size_t value) { return value != 0; });
  Image output(image.width(), image.height(), Color{});
  for (std::size_t y = 0; y < image.height(); ++y) {
    for (std::size_t x = 0; x < image.width(); ++x) {
      const Color source = image.At(x, y);
      const std::size_t luminance = static_cast<std::size_t>((static_cast<unsigned>(source.r) + source.g + source.b) / 3);
      const std::size_t equalized = total > cdf_min ? ((cdf[luminance] - cdf_min) * 255) / (total - cdf_min) : luminance;
      output.SetPixel(x, y, MakeColor(static_cast<int>(equalized), static_cast<int>(equalized), static_cast<int>(equalized), source.a));
    }
  }
  return output;
}

Image Posterize(const Image& image, std::size_t levels) {
  if (levels < 2) {
    return image;
  }
  const double step = 255.0 / static_cast<double>(levels - 1);
  Image output(image.width(), image.height(), Color{});
  for (std::size_t y = 0; y < image.height(); ++y) {
    for (std::size_t x = 0; x < image.width(); ++x) {
      const Color source = image.At(x, y);
      auto posterize_channel = [step](uint8_t channel) {
        return ClampToByte(std::round(channel / step) * step);
      };
      output.SetPixel(x, y, Color{posterize_channel(source.r), posterize_channel(source.g), posterize_channel(source.b), source.a});
    }
  }
  return output;
}

Image Threshold(const Image& image, uint8_t cutoff) {
  Image output(image.width(), image.height(), Color{});
  for (std::size_t y = 0; y < image.height(); ++y) {
    for (std::size_t x = 0; x < image.width(); ++x) {
      const Color source = image.At(x, y);
      const unsigned luminance = static_cast<unsigned>(source.r) + source.g + source.b;
      const uint8_t value = luminance / 3U >= cutoff ? 255U : 0U;
      output.SetPixel(x, y, Color{value, value, value, source.a});
    }
  }
  return output;
}

Image Tint(const Image& image, Color tint, double amount) {
  amount = ClampDouble(amount, 0.0, 1.0);
  Image output(image.width(), image.height(), Color{});
  for (std::size_t y = 0; y < image.height(); ++y) {
    for (std::size_t x = 0; x < image.width(); ++x) {
      const Color source = image.At(x, y);
      output.SetPixel(x, y, LerpColor(source, tint, amount));
    }
  }
  return output;
}

Image AutoContrast(const Image& image) {
  if (image.empty()) {
    return image;
  }
  uint8_t min_value = 255;
  uint8_t max_value = 0;
  for (const Color& color : image.pixels()) {
    min_value = std::min({min_value, color.r, color.g, color.b});
    max_value = std::max({max_value, color.r, color.g, color.b});
  }
  if (min_value >= max_value) {
    return image;
  }
  const double scale = 255.0 / static_cast<double>(max_value - min_value);
  Image output(image.width(), image.height(), Color{});
  for (std::size_t y = 0; y < image.height(); ++y) {
    for (std::size_t x = 0; x < image.width(); ++x) {
      const Color source = image.At(x, y);
      auto adjust = [min_value, scale](uint8_t channel) {
        return ClampToByte((static_cast<double>(channel) - min_value) * scale);
      };
      output.SetPixel(x, y, Color{adjust(source.r), adjust(source.g), adjust(source.b), source.a});
    }
  }
  return output;
}

Image CropOpaqueBounds(const Image& image) {
  if (image.empty()) {
    return image;
  }
  std::size_t min_x = image.width();
  std::size_t min_y = image.height();
  std::size_t max_x = 0;
  std::size_t max_y = 0;
  bool found = false;
  for (std::size_t y = 0; y < image.height(); ++y) {
    for (std::size_t x = 0; x < image.width(); ++x) {
      if (image.At(x, y).a == 0) {
        continue;
      }
      found = true;
      min_x = std::min(min_x, x);
      min_y = std::min(min_y, y);
      max_x = std::max(max_x, x);
      max_y = std::max(max_y, y);
    }
  }
  if (!found) {
    return image;
  }
  return image.Crop(min_x, min_y, max_x - min_x + 1, max_y - min_y + 1);
}

Image QuantizeNearest(const Image& image, const std::vector<Color>& palette) {
  if (palette.empty()) {
    return image;
  }
  auto distance = [](const Color& lhs, const Color& rhs) {
    const int dr = static_cast<int>(lhs.r) - rhs.r;
    const int dg = static_cast<int>(lhs.g) - rhs.g;
    const int db = static_cast<int>(lhs.b) - rhs.b;
    return dr * dr + dg * dg + db * db;
  };

  Image output(image.width(), image.height(), Color{});
  for (std::size_t y = 0; y < image.height(); ++y) {
    for (std::size_t x = 0; x < image.width(); ++x) {
      const Color source = image.At(x, y);
      const Color* best = &palette.front();
      int best_distance = distance(source, *best);
      for (const Color& candidate : palette) {
        const int current = distance(source, candidate);
        if (current < best_distance) {
          best_distance = current;
          best = &candidate;
        }
      }
      output.SetPixel(x, y, *best);
    }
  }
  return output;
}

std::vector<Color> ExtractDominantColors(const Image& image, std::size_t max_colors) {
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
  std::unordered_map<Color, std::size_t, Hasher, Equality> counts;
  for (const Color& color : image.pixels()) {
    ++counts[color];
  }
  std::vector<std::pair<Color, std::size_t>> sorted(counts.begin(), counts.end());
  std::sort(sorted.begin(), sorted.end(), [](const auto& lhs, const auto& rhs) {
    if (lhs.second != rhs.second) {
      return lhs.second > rhs.second;
    }
    return lhs.first.r < rhs.first.r;
  });
  std::vector<Color> palette;
  for (const auto& item : sorted) {
    if (palette.size() == max_colors) {
      break;
    }
    palette.push_back(item.first);
  }
  return palette;
}

Image FloydSteinbergDither(const Image& image, const std::vector<Color>& palette) {
  if (image.empty() || palette.empty()) {
    return image;
  }
  Image working = image;
  Image output(image.width(), image.height(), Color{});
  auto nearest = [&palette](const Color& source) {
    const Color* best = &palette.front();
    int best_distance = std::abs(static_cast<int>(source.r) - best->r) + std::abs(static_cast<int>(source.g) - best->g) + std::abs(static_cast<int>(source.b) - best->b);
    for (const Color& candidate : palette) {
      const int distance = std::abs(static_cast<int>(source.r) - candidate.r) + std::abs(static_cast<int>(source.g) - candidate.g) + std::abs(static_cast<int>(source.b) - candidate.b);
      if (distance < best_distance) {
        best_distance = distance;
        best = &candidate;
      }
    }
    return *best;
  };

  auto distribute = [&](int x, int y, double factor, const Color& error) {
    if (x < 0 || y < 0 || static_cast<std::size_t>(x) >= working.width() || static_cast<std::size_t>(y) >= working.height()) {
      return;
    }
    Color pixel = working.At(static_cast<std::size_t>(x), static_cast<std::size_t>(y));
    pixel.r = ClampToByte(pixel.r + error.r * factor);
    pixel.g = ClampToByte(pixel.g + error.g * factor);
    pixel.b = ClampToByte(pixel.b + error.b * factor);
    working.SetPixel(static_cast<std::size_t>(x), static_cast<std::size_t>(y), pixel);
  };

  for (std::size_t y = 0; y < working.height(); ++y) {
    for (std::size_t x = 0; x < working.width(); ++x) {
      const Color original = working.At(x, y);
      const Color quantized = nearest(original);
      output.SetPixel(x, y, quantized);
      const Color error{
          static_cast<uint8_t>(static_cast<int>(original.r) - quantized.r),
          static_cast<uint8_t>(static_cast<int>(original.g) - quantized.g),
          static_cast<uint8_t>(static_cast<int>(original.b) - quantized.b),
          0,
      };
      distribute(static_cast<int>(x) + 1, static_cast<int>(y), 7.0 / 16.0, error);
      distribute(static_cast<int>(x) - 1, static_cast<int>(y) + 1, 3.0 / 16.0, error);
      distribute(static_cast<int>(x), static_cast<int>(y) + 1, 5.0 / 16.0, error);
      distribute(static_cast<int>(x) + 1, static_cast<int>(y) + 1, 1.0 / 16.0, error);
    }
  }
  return output;
}

Image Checkerboard(std::size_t width, std::size_t height, Color a, Color b, std::size_t cell_size) {
  Image image(width, height, a);
  if (cell_size == 0) {
    cell_size = 1;
  }
  for (std::size_t y = 0; y < height; ++y) {
    for (std::size_t x = 0; x < width; ++x) {
      const bool use_b = ((x / cell_size) + (y / cell_size)) % 2 == 1;
      image.SetPixel(x, y, use_b ? b : a);
    }
  }
  return image;
}

}  // namespace mosaic
