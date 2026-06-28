#include "mosaic/sprite_sheet.h"

#include <algorithm>
#include <sstream>

#include "mosaic/scanner.h"

namespace mosaic {

bool SpriteSheet::Load(const std::string& text, std::string* error) {
  Scanner scanner(text);
  if (!scanner.Consume("sheet")) {
    if (error) *error = "missing sheet header";
    return false;
  }
  if (!scanner.NextToken(&name_) || name_.empty()) {
    if (error) *error = "missing sheet name";
    return false;
  }
  if (!scanner.NextUnsigned(&tile_width_) || !scanner.NextUnsigned(&tile_height_) || tile_width_ == 0 || tile_height_ == 0) {
    if (error) *error = "invalid tile dimensions";
    return false;
  }
  if (!scanner.NextUnsigned(&tile_count_) || tile_count_ == 0) {
    if (error) *error = "invalid tile count";
    return false;
  }
  std::string token;
  while (scanner.NextToken(&token)) {
    payload_.append(token);
    payload_.push_back(' ');
  }
  if (payload_.empty()) {
    if (error) *error = "missing payload";
    return false;
  }
  return true;
}

std::string SpriteSheet::ContactSheet() const {
  std::ostringstream out;
  out << name_ << ':' << tile_width_ << 'x' << tile_height_ << ':' << tile_count_;
  if (!payload_.empty()) {
    out << ':' << payload_.substr(0, std::min<std::size_t>(payload_.size(), tile_count_));
  }
  if (tile_count_ > 3 && !payload_.empty()) {
    std::size_t pivot = tile_count_ * tile_width_;
    out << payload_[pivot % payload_.size()];
  }
  return out.str();
}

}  // namespace mosaic
