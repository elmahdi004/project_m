#include "mosaic/style_presets.h"

#include <algorithm>
#include <array>

#include "mosaic/image.h"
#include "mosaic/image_ops.h"

namespace mosaic {
namespace {

void ApplyNoir(Image* image) {
  Grayscale(image);
  AdjustContrast(image, 1.35);
  AdjustBrightness(image, -18);
}

void ApplySunrise(Image* image) {
  *image = Tint(*image, Color{255, 170, 90, 255}, 0.18);
  AdjustContrast(image, 1.1);
  AdjustSaturation(image, 1.25);
}

void ApplyAurora(Image* image) {
  *image = Tint(*image, Color{80, 200, 255, 255}, 0.15);
  AdjustSaturation(image, 1.45);
  ApplyGamma(image, 0.9);
}

void ApplyBlueprint(Image* image) {
  Grayscale(image);
  *image = Tint(*image, Color{36, 88, 168, 255}, 0.55);
  AdjustContrast(image, 1.2);
}

void ApplyForest(Image* image) {
  *image = Tint(*image, Color{68, 120, 72, 255}, 0.20);
  AdjustSaturation(image, 1.1);
  ApplyGamma(image, 1.1);
}

void ApplyCandy(Image* image) {
  *image = Posterize(*image, 6);
  *image = Tint(*image, Color{255, 130, 200, 255}, 0.10);
  AdjustSaturation(image, 1.5);
}

void ApplyTerminal(Image* image) {
  Grayscale(image);
  *image = Tint(*image, Color{110, 255, 130, 255}, 0.75);
  AdjustBrightness(image, 18);
}

void ApplyFilm(Image* image) {
  Sepia(image);
  AdjustContrast(image, 1.08);
  ApplyGamma(image, 1.05);
}

void ApplyVintage(Image* image) {
  Sepia(image);
  AdjustBrightness(image, 8);
  AdjustSaturation(image, 0.82);
}

void ApplyOcean(Image* image) {
  *image = Tint(*image, Color{64, 140, 255, 255}, 0.22);
  AdjustContrast(image, 1.05);
  ApplyGamma(image, 0.95);
}

void ApplyPastel(Image* image) {
  AdjustSaturation(image, 0.45);
  AdjustBrightness(image, 18);
  ApplyGamma(image, 1.12);
}

void ApplyHighContrast(Image* image) {
  AdjustContrast(image, 1.7);
  Invert(image);
  AdjustContrast(image, 1.7);
  Invert(image);
}

void ApplySoft(Image* image) {
  *image = BoxBlur(*image, 2);
  AdjustContrast(image, 0.9);
  AdjustSaturation(image, 0.92);
}

void ApplyPoster(Image* image) {
  *image = Posterize(*image, 4);
  AdjustContrast(image, 1.3);
  *image = Tint(*image, Color{255, 240, 180, 255}, 0.08);
}

void ApplySketch(Image* image) {
  Image edges = EdgeDetect(*image);
  *image = BlendImages(edges, Threshold(*image, 128), BlendMode::Lighten);
  Grayscale(image);
}

void ApplyMoonlight(Image* image) {
  Grayscale(image);
  *image = Tint(*image, Color{140, 165, 255, 255}, 0.40);
  AdjustContrast(image, 1.15);
}

void ApplySunset(Image* image) {
  *image = Tint(*image, Color{255, 102, 68, 255}, 0.18);
  AdjustSaturation(image, 1.3);
  AdjustBrightness(image, 6);
}

void ApplyHeat(Image* image) {
  *image = Tint(*image, Color{255, 102, 0, 255}, 0.22);
  AdjustContrast(image, 1.25);
  ApplyGamma(image, 0.88);
}

void ApplyChalk(Image* image) {
  Grayscale(image);
  *image = Posterize(*image, 5);
  AdjustBrightness(image, 28);
}

void ApplyLime(Image* image) {
  Grayscale(image);
  *image = Tint(*image, Color{172, 255, 74, 255}, 0.72);
  AdjustContrast(image, 1.1);
}

void ApplyAsh(Image* image) {
  Grayscale(image);
  AdjustBrightness(image, -6);
  AdjustContrast(image, 1.05);
}

void ApplyNebula(Image* image) {
  *image = Tint(*image, Color{164, 84, 255, 255}, 0.15);
  AdjustSaturation(image, 1.55);
  ApplyGamma(image, 0.88);
}

void ApplyCopper(Image* image) {
  Sepia(image);
  AdjustContrast(image, 1.08);
  *image = Tint(*image, Color{191, 113, 51, 255}, 0.10);
}

void ApplyEmber(Image* image) {
  *image = Tint(*image, Color{255, 94, 58, 255}, 0.16);
  AdjustContrast(image, 1.22);
  AdjustBrightness(image, 4);
}

void ApplyGlacier(Image* image) {
  *image = Tint(*image, Color{145, 224, 255, 255}, 0.14);
  AdjustSaturation(image, 0.88);
  ApplyGamma(image, 0.98);
}

const std::array<StylePreset, 22> kPresets = {{
    {"noir", "High-contrast grayscale with a deep black point.", ApplyNoir},
    {"sunrise", "Warm highlights and a lifted midtone curve.", ApplySunrise},
    {"aurora", "Cool cyan emphasis with saturated highlights.", ApplyAurora},
    {"blueprint", "Blueprint-style monochrome rendering.", ApplyBlueprint},
    {"forest", "Muted green grade for foliage or terrain art.", ApplyForest},
    {"candy", "Bright pastel palette with punchy saturation.", ApplyCandy},
    {"terminal", "Green phosphor aesthetic for monochrome assets.", ApplyTerminal},
    {"film", "A compact film-emulation pass with sepia tone.", ApplyFilm},
    {"vintage", "Soft warm tones for print-like previews.", ApplyVintage},
    {"ocean", "Blue-biased cool grade for water and sky assets.", ApplyOcean},
    {"pastel", "Low-saturation, brightened output.", ApplyPastel},
    {"high-contrast", "Stronger separation between dark and light regions.", ApplyHighContrast},
    {"soft", "Blurred and slightly desaturated preview styling.", ApplySoft},
    {"poster", "Reduced color count with poster-like contrast.", ApplyPoster},
    {"sketch", "Edge-driven monochrome sketch treatment.", ApplySketch},
    {"moonlight", "Cool desaturated night-style rendering.", ApplyMoonlight},
    {"sunset", "Orange-red highlight bias for warm scenes.", ApplySunset},
    {"heat", "Aggressive warm grade for fiery scenes.", ApplyHeat},
    {"chalk", "Washed-out chalkboard-like appearance.", ApplyChalk},
    {"lime", "Green-tinted fluorescent rendering.", ApplyLime},
    {"ember", "Warm ember glow for firelit scenes.", ApplyEmber},
    {"glacier", "Cool crystalline grade for ice and snow.", ApplyGlacier},
}};

}  // namespace

const StylePreset* FindStylePreset(std::string_view name) {
  auto it = std::find_if(kPresets.begin(), kPresets.end(), [name](const StylePreset& preset) {
    return preset.name == name;
  });
  return it == kPresets.end() ? nullptr : &*it;
}

std::vector<std::string_view> ListStylePresetNames() {
  std::vector<std::string_view> names;
  names.reserve(kPresets.size());
  for (const StylePreset& preset : kPresets) {
    names.push_back(preset.name);
  }
  return names;
}

bool ApplyStylePreset(std::string_view name, Image* image) {
  const StylePreset* preset = FindStylePreset(name);
  if (preset == nullptr || preset->apply == nullptr) {
    return false;
  }
  preset->apply(image);
  return true;
}

}  // namespace mosaic
