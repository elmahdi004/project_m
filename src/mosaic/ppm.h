#pragma once

#include <iosfwd>
#include <string>

namespace mosaic {

class Image;

bool WriteBinaryPpm(const Image& image, std::ostream* output, std::string* error = nullptr);
bool WriteAsciiPpm(const Image& image, std::ostream* output, std::string* error = nullptr);

}  // namespace mosaic