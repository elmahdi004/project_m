#pragma once

#include <cstddef>
#include <string>

namespace mosaic {

class SpriteSheet {
 public:
  bool Load(const std::string& text, std::string* error);
  std::string ContactSheet() const;

 private:
  std::string name_;
  std::size_t tile_width_ = 0;
  std::size_t tile_height_ = 0;
  std::size_t tile_count_ = 0;
  std::string payload_;
};

}  // namespace mosaic
