#include "structure.h"
#include "setup.h"
#include "file_util.h"

std::string g_source;
ObjectManager g_manager;
Window g_window;
Time g_time;
InputManager g_keyboard, g_mouse;

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
void object::render()
{
    g_window.screen.store();
    g_manager.render();
    g_window.screen.draw();
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

std::unordered_map<std::string, Mesh> g_loadedMeshes;

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

std::unordered_map<std::string, uint32_t> g_loadedTextures;

void texture::set(const std::string &path, int32_t screenChannel)
{
    int32_t width, height, channels;
    unsigned char *data = stbi_load((g_source + "resources/images/" + path).c_str(), &width, &height, &channels, 0);

    uint32_t channel;
    switch (channels)
    {
    case 1:
        channel = GL_RED;
        break;
    case 3:
        channel = GL_RGB;
        break;
    case 4:
        channel = GL_RGBA;
        break;
    }
    uint32_t texture;

    glGenTextures(1, &texture);

    if (data)
    {
        glBindTexture(GL_TEXTURE_2D, texture);
        glTexImage2D(GL_TEXTURE_2D, 0, screenChannel, width, height, 0, channel, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);

        // set the texture wrapping/filtering options (on the currently bound texture object)
        glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAX_ANISOTROPY, 16);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    }
    else
    {
        std::cout << "Failed to load texture: " << path << std::endl;
    }
    stbi_image_free(data);
    g_loadedTextures[path] = texture;
}
void texture::set(const std::string &path, const std::vector<std::string> &subPaths, const std::string &type, uint32_t screenChannel)
{
    for (std::string subPath : subPaths)
    {
        texture::set(path + subPath + "." + type, GL_SRGB_ALPHA);
    }
}
uint32_t texture::get(const std::string &path)
{
    return g_loadedTextures.at(path);
}
std::vector<uint32_t> texture::get(const std::string &path, const std::vector<std::string> &subPaths, const std::string &type)
{
    std::vector<uint32_t> textures;
    for (std::string subPath : subPaths)
    {
        textures.push_back(g_loadedTextures[path + subPath + "." + type]);
    }
    return textures;
}
void texture::remove()
{
    for (auto &pair : g_loadedTextures)
    {
        glDeleteTextures(1, &pair.second);
    }
}

std::unordered_map<std::string, Shader> g_loadedShaders;

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
