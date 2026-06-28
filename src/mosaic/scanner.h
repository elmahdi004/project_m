#pragma once

#include <cstddef>
#include <string>

#include "mosaic/color.h"

namespace mosaic {

class Scanner {
 public:
  explicit Scanner(std::string input);

  bool NextToken(std::string* token);
  bool NextUnsigned(std::size_t* value);
  bool NextInt(int* value);
  bool NextColor(Color* color);
  bool Consume(const std::string& expected);
  bool Done() const;

 private:
  void SkipWhitespace();

  std::string input_;
  std::size_t pos_ = 0;
};

}  // namespace mosaic
