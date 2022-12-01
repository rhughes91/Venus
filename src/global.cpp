#include "file_util.h"
#include "shader.h"
#include "structure.h"

Time g_time;
ObjectManager g_manager;
InputManager g_keyboard, g_mouse;

std::string g_source;
std::unordered_map<std::string, Mesh> g_loadedMeshes;
std::unordered_map<std::string, uint32_t> g_loadedTextures;
std::unordered_map<std::string, Shader> g_loadedShaders;

void object::load()
{
    g_manager.load();
}
void object::start()
{
    g_manager.start();
}
void object::destroy()
{
    g_manager.destroy();
}
void object::update()
{
    g_manager.update();
}
void object::lateUpdate()
{
    g_manager.lateUpdate();
}
void object::fixedUpdate()
{
    g_manager.fixedUpdate();
}

Mesh &mesh::set(const std::string &path)
{
    return(g_loadedMeshes[path] = loadObjFile(path, 1));
}
Mesh &mesh::set(const std::string& path, const Mesh& mesh)
{
    return(g_loadedMeshes[path] = mesh);
}
void mesh::set(const std::string &path, const std::vector<std::string> &subPaths, const std::string &type)
{
    for (std::string subPath : subPaths)
    {
        mesh::set(path + subPath + "." + type);
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

Shader &shader::set(const std::string& path, const Shader& shader)
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
