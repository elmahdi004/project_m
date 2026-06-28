#include "mosaic/cli.h"

#include <fstream>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>

#include "mosaic/image.h"
#include "mosaic/image_ops.h"
#include "mosaic/montage.h"
#include "mosaic/ppm.h"
#include "mosaic/report.h"
#include "mosaic/style_presets.h"
#include "mosaic/scene_io.h"
#include "mosaic/sprite_sheet.h"

namespace mosaic {
namespace {

struct Options {
  std::string command;
  std::string input_path;
  std::string output_path;
  std::string preset_name;
  bool ascii = false;
};

bool WriteFile(const std::string& path, const std::string& contents, std::string* error) {
  std::ofstream output(path, std::ios::binary);
  if (!output) {
    if (error) *error = "unable to open output file";
    return false;
  }
  output << contents;
  if (!output.good()) {
    if (error) *error = "unable to write output file";
    return false;
  }
  return true;
}

bool ParseOptions(int argc, char* argv[], Options* options, std::string* error) {
  if (argc < 3) {
    if (error) *error = "usage: mosaicc <inspect|render|spritesheet|montage> <input> [output] [--ascii]";
    return false;
  }
  options->command = argv[1];
  options->input_path = argv[2];
  for (int i = 3; i < argc; ++i) {
    const std::string argument = argv[i];
    if (argument == "--ascii") {
      options->ascii = true;
    } else if (argument == "--preset" && i + 1 < argc) {
      options->preset_name = argv[++i];
    } else if (options->output_path.empty()) {
      options->output_path = argument;
    } else {
      if (error) *error = "too many arguments";
      return false;
    }
  }
  return true;
}

bool RunInspect(const Options& options, std::string* error) {
  Scene scene;
  std::string parse_error;
  Image image;
  if (!LoadSceneFile(options.input_path, &scene, &image, &parse_error)) {
    if (error) *error = parse_error;
    return false;
  }
  if (!options.preset_name.empty() && !ApplyStylePreset(options.preset_name, &image)) {
    if (error) *error = "unknown style preset";
    return false;
  }
  std::cout << FormatSceneReport(BuildSceneReport(scene, image)) << std::endl;
  return true;
}

bool RunRender(const Options& options, std::string* error) {
  Scene scene;
  Image image;
  if (!LoadSceneFile(options.input_path, &scene, &image, error)) {
    return false;
  }
  if (!options.preset_name.empty() && !ApplyStylePreset(options.preset_name, &image)) {
    if (error) *error = "unknown style preset";
    return false;
  }
  std::ostringstream rendered;
  if (options.ascii || options.output_path.empty()) {
    rendered << image.AsciiPreview();
  } else if (options.output_path.size() >= 4 && options.output_path.substr(options.output_path.size() - 4) == ".ppm") {
    if (!WriteBinaryPpm(image, &rendered, error)) {
      return false;
    }
  } else {
    if (!WriteAsciiPpm(image, &rendered, error)) {
      return false;
    }
  }

  if (options.output_path.empty()) {
    std::cout << rendered.str();
    return true;
  }
  return WriteFile(options.output_path, rendered.str(), error);
}

bool RunMontage(const Options& options, std::string* error) {
  Image montage = BuildMontageFromDirectory(options.input_path, error);
  if (montage.empty()) {
    return false;
  }

  if (!options.preset_name.empty() && !ApplyStylePreset(options.preset_name, &montage)) {
    if (error) *error = "unknown style preset";
    return false;
  }

  if (options.ascii || options.output_path.empty()) {
    std::string preview = montage.AsciiPreview();
    if (options.output_path.empty()) {
      std::cout << preview;
      return true;
    }
    return WriteFile(options.output_path, preview, error);
  }

  std::ostringstream rendered;
  if (!WriteBinaryPpm(montage, &rendered, error)) {
    return false;
  }
  return WriteFile(options.output_path, rendered.str(), error);
}

bool RunSpriteSheetCommand(const Options& options, std::string* error) {
  std::string contents;
  if (!LoadTextFile(options.input_path, &contents, error)) {
    return false;
  }
  SpriteSheet sheet;
  std::string parse_error;
  if (!sheet.Load(contents, &parse_error)) {
    if (error) *error = parse_error;
    return false;
  }
  std::cout << DescribeSpriteSheet(sheet) << std::endl;
  return true;
}

}  // namespace

int RunCli(int argc, char* argv[]) {
  Options options;
  std::string error;
  if (!ParseOptions(argc, argv, &options, &error)) {
    std::cerr << error << std::endl;
    return 1;
  }
  if (options.command == "inspect") {
    return RunInspect(options, &error) ? 0 : (std::cerr << error << std::endl, 1);
  }
  if (options.command == "render") {
    return RunRender(options, &error) ? 0 : (std::cerr << error << std::endl, 1);
  }
  if (options.command == "spritesheet") {
    return RunSpriteSheetCommand(options, &error) ? 0 : (std::cerr << error << std::endl, 1);
  }
  if (options.command == "montage") {
    return RunMontage(options, &error) ? 0 : (std::cerr << error << std::endl, 1);
  }
  std::cerr << "unknown command" << std::endl;
  return 1;
}

}  // namespace mosaic