#ifndef FILEUTIL_H
#define FILEUTIL_H

#include <string>
#include <vector>

// loads file at 'fileName' into a std::string
std::string loadFileToString(const std::string &fileName);

// loads file at 'fileName' into std::string vector separated by line
std::vector<std::string> loadFileToStringVector(const std::string &fileName);

std::string getCurrentDirectoryName();

void loadTTF(const std::string &fileName);

#endif