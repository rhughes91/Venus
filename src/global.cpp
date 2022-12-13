#include "file_util.h"
#include "shader.h"
#include "structure.h"
#include <windows.h>

Time g_time;

float event::delta()
{
    return g_time.deltaTime;
}
float event::time()
{
    return g_time.runtime;
}

std::unordered_map<std::string, Mesh> g_loadedMeshes;
std::unordered_map<std::string, uint32_t> g_loadedTextures;
std::unordered_map<std::string, Shader> g_loadedShaders;

Mesh &mesh::load(const std::string &path)
{
    return(g_loadedMeshes[path] = loadObjFile(path, 1));
}
Mesh &mesh::load(const std::string& path, const Mesh& mesh)
{
    return(g_loadedMeshes[path] = mesh);
}
void mesh::load(const std::string &path, const std::vector<std::string> &subPaths, const std::string &type)
{
    for (std::string subPath : subPaths)
    {
        mesh::load(path + subPath + "." + type);
    }
}
Mesh &mesh::get(const std::string &path)
{
    return g_loadedMeshes.at(path);
}
std::vector<Mesh> mesh::get(const std::string &path, const std::vector<std::string> &subPaths, const std::string &type)
{
    std::vector<Mesh> meshes;
    for (std::string subPath : subPaths)
    {
        meshes.push_back(g_loadedMeshes[path + subPath + "." + type]);
    }
    return meshes;
}
void mesh::remove()
{
    for (auto &pair : g_loadedMeshes)
    {
        pair.second.remove();
    }
}

Shader &shader::load(const std::string& path, const Shader& shader)
{
    return(g_loadedShaders[path] = shader);
}
Shader &shader::get(const std::string& path)
{
    return g_loadedShaders.at(path);
}
void shader::remove()
{
    for (auto &pair : g_loadedShaders)
    {
        pair.second.remove();
    }
}

