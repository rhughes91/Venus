#ifndef GRAPHICS_H
#define GRAPHICS_H

#include <vector>
#include <unordered_map>

#include "vector.h"
#include "shader.h"

// TextureBuffer (struct): wrapper for graphical texture data
struct TextureBuffer
{
    uint32_t data, type;
};

// FrameBuffer (struct): wrapper for graphical framebuffer data :: allows for simpler editing of the framebuffer data
struct FrameBuffer
{
    uint32_t data;
    std::unordered_map<std::string, uint32_t> renderBuffers;
    std::unordered_map<std::string, TextureBuffer> textures;

    void initialize();
    void remove();
    void refresh(uint16_t width, uint16_t height);
    bool complete();

    void addTexture(const std::string& name, uint16_t width, uint16_t height, uint32_t component, uint32_t componentType, uint32_t attachment, uint32_t scaling, uint32_t wrapping, bool multisampled);
    void addRenderBuffer(const std::string& name);

    void bind(uint32_t type);
    void unbind();
    void bindTexture(const std::string& name);
    TextureBuffer getTexture(const std::string& name)
    {
        return textures[name];
    }
};

// Vertex (struct): holds basic vertex data as it is stored in a .obj file
struct Vertex
{
    Vector3 position;
    Vector3 normal;
    Vector2 uv;
};

// Mesh (struct): wrapper for the Vertex Array Object and Vertex Buffer Object necessary to render a mesh
class Mesh
{
    public:
        Mesh() {}
        Mesh(Vector3 vertices__[], uint32_t numVertices, float texture__[], const Vector3& dimensions__);
        Mesh(const std::vector<Vertex> &vertices, const Vector3& dimensions__);

        void draw(const uint32_t texture) const;
        void remove();

        uint32_t VAO, VBO, count;
        Vector3 dimensions;
};

// shape (namespace): provides basic Mesh shapes without needing to load a file
namespace shape
{
    Mesh square(int32_t tiling = 1);
    Mesh cube();
}

// mesh (namespace): global methods for loading and accessing mesh data from .obj files
namespace mesh
{
    Mesh &set(const std::string& path);
    Mesh &set(const std::string& path, const Mesh& mesh);
    void set(const std::string& path, const std::vector<std::string>& subPaths, const std::string& type);
    Mesh &get(const std::string& path);
    std::vector<Mesh> get(const std::string& path, const std::vector<std::string>& subPaths, const std::string& type);

    void remove();
};

// texture (namespace): global methods for loading and accessing image data from image files
namespace texture
{
    void set(const std::string& path, int32_t screenChannel);
    void set(const std::string& path, const std::vector<std::string>& subPaths, const std::string& type, uint32_t screenChannel);
    uint32_t get(const std::string& path);
    std::vector<uint32_t> get(const std::string& path, const std::vector<std::string>& subPaths, const std::string& type);

    void remove();
};

#endif