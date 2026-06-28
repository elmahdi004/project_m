#include "mosaic/montage.h"

#include <algorithm>
#include <cmath>
#include <string>
#include <utility>

#include "mosaic/image.h"
#include "mosaic/image_ops.h"
#include "mosaic/scene.h"
#include "mosaic/scene_io.h"

namespace mosaic {
namespace {

std::size_t ChooseColumns(std::size_t count) {
  if (count == 0) {
    return 1;
  }
  const std::size_t square = static_cast<std::size_t>(std::ceil(std::sqrt(static_cast<double>(count))));
  return std::max<std::size_t>(1, square);
}

Image FrameThumbnail(const Image& image, const MontageLayout& layout) {
  Image framed = Pad(image, 1, 1, 1, 1, layout.border);
  if (framed.width() < 4 || framed.height() < 4) {
    return framed;
  }
  DrawRectangle(&framed, 0, 0, static_cast<int>(framed.width()), static_cast<int>(framed.height()), layout.border, false);
  return framed;
}

}  // namespace

Image BuildMontage(const std::vector<Image>& images, const MontageLayout& layout) {
  if (images.empty()) {
    return Image(1, 1, layout.background);
  }

  const std::size_t columns = layout.columns == 0 ? ChooseColumns(images.size()) : layout.columns;
  std::size_t row_height = 0;
  std::size_t max_width = 0;
  std::vector<Image> thumbnails;
  thumbnails.reserve(images.size());

  for (const Image& image : images) {
    Image thumbnail = image.ResizedNearest(std::max<std::size_t>(1, std::min<std::size_t>(image.width(), 64)),
                                           std::max<std::size_t>(1, std::min<std::size_t>(image.height(), 64)));
    thumbnail = FrameThumbnail(thumbnail, layout);
    row_height = std::max(row_height, thumbnail.height());
    max_width = std::max(max_width, thumbnail.width());
    thumbnails.push_back(std::move(thumbnail));
  }

  const std::size_t rows = (thumbnails.size() + columns - 1) / columns;
  const std::size_t cell_width = max_width + layout.padding;
  const std::size_t cell_height = row_height + layout.padding;
  const std::size_t total_width = columns * cell_width + layout.padding;
  const std::size_t total_height = rows * cell_height + layout.padding;

  Image canvas(total_width, total_height, layout.background);
  for (std::size_t index = 0; index < thumbnails.size(); ++index) {
    const std::size_t column = index % columns;
    const std::size_t row = index / columns;
    const std::size_t origin_x = layout.padding + column * cell_width;
    const std::size_t origin_y = layout.padding + row * cell_height;
    const Image& thumbnail = thumbnails[index];
    for (std::size_t y = 0; y < thumbnail.height(); ++y) {
      for (std::size_t x = 0; x < thumbnail.width(); ++x) {
        const std::size_t target_x = origin_x + x;
        const std::size_t target_y = origin_y + y;
        if (target_x < canvas.width() && target_y < canvas.height()) {
          canvas.SetPixel(target_x, target_y, thumbnail.At(x, y));
        }
      }
    }
  }

  return canvas;
}

Image BuildMontageFromDirectory(const std::string& directory, std::string* error, const MontageLayout& layout) {
  std::vector<std::string> files = CollectSceneFiles(directory, error);
  if (files.empty()) {
    return Image{};
  }

  std::vector<Image> images;
  images.reserve(files.size());
  for (const std::string& file : files) {
    Scene scene;
    Image image;
    if (!LoadSceneFile(file, &scene, &image, error)) {
      return Image{};
    }
    images.push_back(std::move(image));
  }

  return BuildMontage(images, layout);
}

}  // namespace mosaic
