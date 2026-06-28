#include "mosaic/scene_io.h"

#include <algorithm>
#include <fstream>
#include <sstream>
#include <string>

#include <filesystem>

#include "mosaic/image.h"
#include "mosaic/scanner.h"
#include "mosaic/scene.h"

namespace mosaic {
namespace fs = std::filesystem;

bool LoadTextFile(const std::string& path, std::string* contents, std::string* error) {
  std::ifstream input(path, std::ios::binary);
  if (!input) {
    if (error) *error = "unable to open input file";
    return false;
  }
  std::ostringstream buffer;
  buffer << input.rdbuf();
  if (!input.good() && !input.eof()) {
    if (error) *error = "unable to read input file";
    return false;
  }
  *contents = buffer.str();
  return true;
}

bool LoadSceneFile(const std::string& path, Scene* scene, Image* image, std::string* error) {
  std::string contents;
  if (!LoadTextFile(path, &contents, error)) {
    return false;
  }
  Scanner scanner(contents);
  std::string parse_error;
  if (!scene->Parse(&scanner, &parse_error)) {
    if (error) *error = parse_error;
    return false;
  }
  *image = Image(scene->width(), scene->height());
  image->ReplacePixels(scene->width(), scene->height(), scene->Composite());
  return true;
}

std::vector<std::string> CollectSceneFiles(const std::string& directory, std::string* error) {
  std::vector<std::string> files;
  std::error_code ec;
  if (!fs::exists(directory, ec)) {
    if (error) *error = "directory does not exist";
    return files;
  }
  if (!fs::is_directory(directory, ec)) {
    if (error) *error = "path is not a directory";
    return files;
  }

  for (const fs::directory_entry& entry : fs::directory_iterator(directory, ec)) {
    if (ec) {
      if (error) *error = "unable to enumerate directory";
      return {};
    }
    if (!entry.is_regular_file()) {
      continue;
    }
    const fs::path path = entry.path();
    const std::string extension = path.extension().string();
    if (extension == ".mosaic" || extension == ".scene" || extension == ".txt") {
      files.push_back(path.string());
    }
  }
  std::sort(files.begin(), files.end());
  return files;
}

}  // namespace mosaic
