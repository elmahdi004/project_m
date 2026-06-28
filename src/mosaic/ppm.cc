#include "mosaic/ppm.h"

#include <ostream>
#include <string>

#include "mosaic/image.h"

namespace mosaic {
namespace {

bool WriteHeader(const Image& image, std::ostream* output, const char* magic, std::string* error) {
  if (image.width() == 0 || image.height() == 0) {
    if (error) {
      *error = "image is empty";
    }
    return false;
  }
  (*output) << magic << '\n' << image.width() << ' ' << image.height() << '\n' << 255 << '\n';
  return static_cast<bool>(*output);
}

}  // namespace

bool WriteBinaryPpm(const Image& image, std::ostream* output, std::string* error) {
  if (!WriteHeader(image, output, "P6", error)) {
    return false;
  }
  for (const Color& color : image.pixels()) {
    output->put(static_cast<char>(color.r));
    output->put(static_cast<char>(color.g));
    output->put(static_cast<char>(color.b));
  }
  return static_cast<bool>(*output);
}

bool WriteAsciiPpm(const Image& image, std::ostream* output, std::string* error) {
  if (!WriteHeader(image, output, "P3", error)) {
    return false;
  }
  for (std::size_t y = 0; y < image.height(); ++y) {
    for (std::size_t x = 0; x < image.width(); ++x) {
      const Color& color = image.At(x, y);
      (*output) << static_cast<unsigned>(color.r) << ' '
                << static_cast<unsigned>(color.g) << ' '
                << static_cast<unsigned>(color.b);
      if (x + 1 < image.width()) {
        (*output) << ' ';
      }
    }
    (*output) << '\n';
  }
  return static_cast<bool>(*output);
}

}  // namespace mosaic