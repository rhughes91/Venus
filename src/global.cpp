#include "file_util.h"
#include "audio.h"
#include "setup.h"
#include "shader.h"
#include "structure.h"

#include <iostream>
#include <windows.h>

Time g_time;
uint32_t currentWindow = 0;
std::vector<Window> g_windows;

std::unordered_map<std::string, Texture> g_loadedTextures;
std::unordered_map<std::string, Audio> g_loadedAudios;

object::ecs g_manager;
ProjectManager g_handler;