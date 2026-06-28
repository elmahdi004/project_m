#include <cstddef>
#include <cstdint>
#include <string>

#include "mosaic/sprite_sheet.h"

extern "C" int LLVMFuzzerTestOneInput(const uint8_t* data, size_t size) {
  std::string input(reinterpret_cast<const char*>(data), size);
  mosaic::SpriteSheet sheet;
  std::string error;
  if (!sheet.Load(input, &error)) {
    return 0;
  }
  (void)sheet.ContactSheet();
  return 0;
}
