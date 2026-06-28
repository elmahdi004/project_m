#pragma once

#include <string>
#include <vector>

namespace mosaic {

class Image;
class Scene;

bool LoadTextFile(const std::string& path, std::string* contents, std::string* error);
bool LoadSceneFile(const std::string& path, Scene* scene, Image* image, std::string* error);
std::vector<std::string> CollectSceneFiles(const std::string& directory, std::string* error);

}  // namespace mosaic
