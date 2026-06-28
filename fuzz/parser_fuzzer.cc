#include <cstddef>
#include <cstdint>
#include <string>

#include "mosaic/palette.h"
#include "mosaic/rle.h"
#include "mosaic/scanner.h"
#include "mosaic/scene.h"

extern "C" int LLVMFuzzerTestOneInput(const uint8_t* data, size_t size) {
  std::string input(reinterpret_cast<const char*>(data), size);
  mosaic::Scanner scanner(input);
  mosaic::Scene scene;
  std::string error;
  if (!scene.Parse(&scanner, &error)) {
    return 0;
  }
  (void)scene.Composite();
  return 0;
}
