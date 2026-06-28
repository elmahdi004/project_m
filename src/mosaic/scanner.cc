#include "mosaic/scanner.h"

#include <cctype>
#include <cstdlib>
#include <limits>
#include <stdexcept>
#include <utility>

namespace mosaic {
namespace {

bool ParseHexDigit(char ch, uint8_t* value) {
  if (ch >= '0' && ch <= '9') {
    *value = static_cast<uint8_t>(ch - '0');
    return true;
  }
  if (ch >= 'a' && ch <= 'f') {
    *value = static_cast<uint8_t>(10 + (ch - 'a'));
    return true;
  }
  if (ch >= 'A' && ch <= 'F') {
    *value = static_cast<uint8_t>(10 + (ch - 'A'));
    return true;
  }
  return false;
}

bool ParseHexByte(char high, char low, uint8_t* value) {
  uint8_t hi = 0;
  uint8_t lo = 0;
  if (!ParseHexDigit(high, &hi) || !ParseHexDigit(low, &lo)) {
    return false;
  }
  *value = static_cast<uint8_t>((hi << 4) | lo);
  return true;
}

bool ParseUnsignedToken(const std::string& token, std::size_t* value) {
  if (token.empty() || token[0] == '-') {
    return false;
  }
  char* end = nullptr;
  unsigned long long parsed = std::strtoull(token.c_str(), &end, 10);
  if (end == token.c_str() || *end != '\0' || parsed > std::numeric_limits<std::size_t>::max()) {
    return false;
  }
  *value = static_cast<std::size_t>(parsed);
  return true;
}

bool ParseIntToken(const std::string& token, int* value) {
  char* end = nullptr;
  long parsed = std::strtol(token.c_str(), &end, 10);
  if (end == token.c_str() || *end != '\0' || parsed < std::numeric_limits<int>::min() || parsed > std::numeric_limits<int>::max()) {
    return false;
  }
  *value = static_cast<int>(parsed);
  return true;
}

}  // namespace

Scanner::Scanner(std::string input) : input_(std::move(input)) {}

void Scanner::SkipWhitespace() {
  while (pos_ < input_.size()) {
    unsigned char ch = static_cast<unsigned char>(input_[pos_]);
    if (!std::isspace(ch)) {
      break;
    }
    ++pos_;
  }
}

bool Scanner::NextToken(std::string* token) {
  SkipWhitespace();
  if (pos_ >= input_.size()) {
    return false;
  }
  std::size_t start = pos_;
  while (pos_ < input_.size()) {
    unsigned char ch = static_cast<unsigned char>(input_[pos_]);
    if (std::isspace(ch)) {
      break;
    }
    ++pos_;
  }
  token->assign(input_.data() + start, pos_ - start);
  return true;
}

bool Scanner::NextUnsigned(std::size_t* value) {
  std::string token;
  if (!NextToken(&token)) {
    return false;
  }
  return ParseUnsignedToken(token, value);
}

bool Scanner::NextInt(int* value) {
  std::string token;
  if (!NextToken(&token)) {
    return false;
  }
  return ParseIntToken(token, value);
}

bool Scanner::NextColor(Color* color) {
  std::string token;
  if (!NextToken(&token)) {
    return false;
  }
  if (!token.empty() && token[0] == '#') {
    token.erase(token.begin());
  }
  if (token.size() != 6 && token.size() != 8) {
    return false;
  }
  if (!ParseHexByte(token[0], token[1], &color->r)) return false;
  if (!ParseHexByte(token[2], token[3], &color->g)) return false;
  if (!ParseHexByte(token[4], token[5], &color->b)) return false;
  color->a = 255;
  if (token.size() == 8 && !ParseHexByte(token[6], token[7], &color->a)) {
    return false;
  }
  return true;
}

bool Scanner::Consume(const std::string& expected) {
  std::string token;
  if (!NextToken(&token)) {
    return false;
  }
  return token == expected;
}

bool Scanner::Done() const {
  std::size_t index = pos_;
  while (index < input_.size()) {
    unsigned char ch = static_cast<unsigned char>(input_[index]);
    if (!std::isspace(ch)) {
      return false;
    }
    ++index;
  }
  return true;
}

}  // namespace mosaic
