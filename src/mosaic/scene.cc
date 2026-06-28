#include "mosaic/scene.h"

#include <algorithm>
#include <utility>

#include "mosaic/palette.h"
#include "mosaic/rle.h"
#include "mosaic/scanner.h"

namespace mosaic {
namespace {

void ReadLayerPixels(Layer* layer, const RleDecoder& decoder, const Palette& palette, std::size_t width, std::size_t height) {
  std::vector<Color> image = decoder.DecodeImage(palette);
  if (image.size() < width * height) {
    image.resize(width * height, palette.Lookup(image.size()));
  }
  layer->pixels = std::move(image);
}

}  // namespace

bool Scene::Parse(Scanner* scanner, std::string* error) {
  layers_.clear();
  background_.clear();
  palette_ = nullptr;
  width_ = 0;
  height_ = 0;

  if (!scanner->Consume("scene")) {
    if (error) *error = "missing scene header";
    return false;
  }

  if (!scanner->NextUnsigned(&width_) || !scanner->NextUnsigned(&height_) || width_ == 0 || height_ == 0) {
    if (error) *error = "invalid scene dimensions";
    return false;
  }

  Palette palette;
  if (!palette.Parse(scanner, error)) {
    return false;
  }
  palette_ = &palette;

  RleDecoder decoder;
  if (!decoder.Parse(scanner, error)) {
    return false;
  }

  std::size_t layer_count = 0;
  if (!scanner->NextUnsigned(&layer_count) || layer_count == 0) {
    if (error) *error = "invalid layer count";
    return false;
  }

  layers_.reserve(layer_count);
  for (std::size_t i = 0; i < layer_count; ++i) {
    Layer layer;
    if (!scanner->NextToken(&layer.name)) {
      if (error) *error = "missing layer name";
      return false;
    }
    int visible = 0;
    if (!scanner->NextInt(&visible)) {
      if (error) *error = "invalid layer visibility";
      return false;
    }
    layer.visible = visible != 0;
    ReadLayerPixels(&layer, decoder, palette, width_, height_);
    layers_.push_back(std::move(layer));
  }

  std::size_t bg_count = 0;
  if (!scanner->NextUnsigned(&bg_count)) {
    if (error) *error = "missing background count";
    return false;
  }
  background_.reserve(bg_count);
  for (std::size_t i = 0; i < bg_count; ++i) {
    Color color;
    if (!scanner->NextColor(&color)) {
      if (error) *error = "invalid background color";
      return false;
    }
    background_.push_back(color);
  }

  return true;
}

std::vector<Color> Scene::Composite() const {
  std::vector<Color> canvas;
  canvas.resize(width_ * height_, background_.empty() ? Color{} : background_.front());

  for (const Layer& layer : layers_) {
    if (!layer.visible || layer.pixels.empty()) {
      continue;
    }
    for (std::size_t i = 0; i < canvas.size() && i < layer.pixels.size(); ++i) {
      canvas[i] = Blend(canvas[i], layer.pixels[i]);
    }
  }

  if (!canvas.empty() && palette_ != nullptr && !background_.empty() && background_.size() > 1) {
    const Color& accent = palette_->Lookup(canvas.size() + background_.size());
    canvas[0] = Blend(canvas[0], accent);
  }

  return canvas;
}

}  // namespace mosaic
