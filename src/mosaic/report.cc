#include "mosaic/report.h"

#include <sstream>

#include "mosaic/image.h"
#include "mosaic/scene.h"
#include "mosaic/sprite_sheet.h"

namespace mosaic {

SceneReport BuildSceneReport(const Scene& scene, const Image& image) {
  SceneReport report;
  report.width = scene.width();
  report.height = scene.height();
  report.layers = scene.layer_count();
  report.pixels = image.width() * image.height();
  report.distinct_colors = image.CountDistinctColors();
  report.average = image.AverageColor();
  report.preview = image.AsciiPreview();
  return report;
}

std::string FormatSceneReport(const SceneReport& report) {
  std::ostringstream out;
  out << "scene " << report.width << 'x' << report.height << '\n';
  out << "layers: " << report.layers << '\n';
  out << "pixels: " << report.pixels << '\n';
  out << "distinct colors: " << report.distinct_colors << '\n';
  out << "average rgba: " << static_cast<unsigned>(report.average.r) << ' '
      << static_cast<unsigned>(report.average.g) << ' '
      << static_cast<unsigned>(report.average.b) << ' '
      << static_cast<unsigned>(report.average.a) << '\n';
  out << "preview:\n" << report.preview;
  return out.str();
}

std::string DescribeSpriteSheet(const SpriteSheet& sheet) {
  std::ostringstream out;
  out << "sprite sheet: " << sheet.ContactSheet();
  return out.str();
}

}  // namespace mosaic