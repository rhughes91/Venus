#ifndef FILEUTIL_H
#define FILEUTIL_H

#include <string>
#include <vector>
#include "graphics.h"

// loads file at 'fileName' into a std::string
std::string loadFileToString(const std::string &fileName);

// loads file at 'fileName' into std::string vector separated by line
std::vector<std::string> loadFileToStringVector(const std::string &fileName);

// loads .obj file at 'filename' :: applied textures will be applied in a checkerboard pattern of size 'tiling'
Mesh loadObjFile(const std::string &fileName, Vector2 tiling = Vector2(1, 1));

#endif