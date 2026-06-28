#include "mosaic/rle.h"

#include <algorithm>

#include "mosaic/palette.h"
#include "mosaic/scanner.h"

namespace mosaic {

bool RleDecoder::Parse(Scanner* scanner, std::string* error) {
  spans_.clear();
  height_ = 0;

  std::string token;
  if (!scanner->NextToken(&token) || token != "rows") {
    if (error) *error = "missing rows header";
    return false;
  }
  if (!scanner->NextUnsigned(&height_) || height_ == 0) {
    if (error) *error = "invalid row count";
    return false;
  }

  std::size_t span_count = 0;
  if (!scanner->NextUnsigned(&span_count) || span_count == 0) {
    if (error) *error = "invalid span count";
    return false;
  }

  spans_.reserve(span_count);
  for (std::size_t i = 0; i < span_count; ++i) {
    Span span;
    if (!scanner->NextUnsigned(&span.count) || span.count == 0) {
      if (error) *error = "invalid span length";
      return false;
    }
    if (!scanner->NextToken(&token)) {
      if (error) *error = "missing span type";
      return false;
    }
    if (token == "ref") {
      if (!scanner->NextUnsigned(&span.palette_index)) {
        if (error) *error = "invalid palette reference";
        return false;
      }
      span.literal = false;
    } else if (token == "lit") {
      if (!scanner->NextColor(&span.literal_color)) {
        if (error) *error = "invalid literal color";
        return false;
      }
      span.literal = true;
    } else {
      if (error) *error = "unknown span type";
      return false;
    }
    spans_.push_back(span);
  }

  return true;
}

std::vector<Color> RleDecoder::DecodeRow(const Palette& palette, std::size_t width) const {
  std::vector<Color> row;
  row.reserve(width);
  std::size_t emitted = 0;

  for (const Span& span : spans_) {
    for (std::size_t i = 0; i < span.count && emitted < width; ++i) {
      if (span.literal) {
        row.push_back(span.literal_color);
      } else {
        row.push_back(palette.Lookup(span.palette_index));
      }
      ++emitted;
    }
    if (emitted >= width) {
      break;
    }
  }

  while (row.size() < width) {
    row.push_back(palette.Lookup(row.size()));
  }
  return row;
}

std::vector<Color> RleDecoder::DecodeImage(const Palette& palette) const {
  std::vector<Color> pixels;
  const std::size_t width = spans_.empty() ? 0 : std::min<std::size_t>(spans_.front().count + 1, 16);
  pixels.reserve(width * height_);

  for (std::size_t y = 0; y < height_; ++y) {
    std::vector<Color> row = DecodeRow(palette, width);
    pixels.insert(pixels.end(), row.begin(), row.end());
  }

  return pixels;
}

}  // namespace mosaic
