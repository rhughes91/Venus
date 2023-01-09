#ifndef SHADER_H
#define SHADER_H

#include <vector>
#include <string>
#include <stdint.h>

#include "vector.h"
#include "color.h"
#include "component.h"

struct Model;

// Shader (struct): wrapper for graphical shader data :: allows for .hlsl files to be updated and used
struct Shader
{
    uint32_t ID;

    Shader() {}
    Shader(std::string vertexPath, std::string fragmentPath);
        
    void initialize(int32_t numPoints, int32_t numSpots);
    void refresh(int32_t numPoints, int32_t numSpots);
    void use() const;
    void remove() const;
    
    void setBool(const std::string &name, bool value) const;  
    void setInt(const std::string &name, int32_t value) const;   
    void setFloat(const std::string &name, float value) const;
    void setVec2(const std::string &name, const Vector2 &vec2) const;
    void setVec3(const std::string &name, const Vector3 &vec3) const;
    void setColor3(const std::string &name, const Color &vec3) const;
    void setVec4(const std::string &name, const Color &vec4) const;
    void setMat4(const std::string &name, const float *, bool) const;

    private:
        std::string m_vertexPath, m_fragmentPath;
        uint32_t compileShader(const std::string &contents, uint32_t type) const;
};

// shader (namespace): global methods for loading and accessing shader data from .hlsl files
namespace shader
{
    Shader &load(const std::string& path, const Shader& shader);
    Shader &get(const std::string& path);
    void remove();
};

// Vertex (struct): holds basic vertex data as it is stored in a .obj file
struct Vertex
{
    Vector3 position;
    Vector3 normal;
    Vector2 uv;
};

// Mesh (struct): wrapper for the Vertex Array Object and Vertex Buffer Object necessary to render a mesh
struct Mesh
{
    std::vector<Vertex> vertices;
    uint32_t VAO, VBO;
    Vector3 dimensions;

    Mesh() {}
    Mesh(Vector3 vertices__[], uint32_t numVertices, float texture__[], const Vector3& dimensions__);
    Mesh(const std::vector<Vertex> &vertices__, const Vector3& dimensions__);

    void append(const Transform& parentTransform, const std::vector<Transform>& additions);
    void refresh();
    void draw(const uint32_t texture) const;
    void remove();
};

struct MeshAddon
{
    std::vector<Transform> additions;

    MeshAddon() {}
    MeshAddon(const std::vector<Transform>& additions__) : additions(additions__) {}
};

// shape (namespace): provides basic Mesh shapes without needing to load a file
namespace shape
{
    Mesh square(int32_t tiling = 1);
    Mesh double_square(int32_t tiling = 1);
    Mesh cube();
}

// mesh (namespace): global methods for loading and accessing mesh data from .obj files
namespace mesh
{
    Mesh &load(const std::string& path);
    Mesh &load(const std::string& path, const Mesh& mesh);
    void load(const std::string& path, const std::vector<std::string>& subPaths, const std::string& type);
    Mesh &get(const std::string& path);
    std::vector<Mesh> get(const std::string& path, const std::vector<std::string>& subPaths, const std::string& type);

    void remove();
};

// loads .obj file at 'filename' :: applied textures will be applied in a checkerboard pattern of size 'tiling'
Mesh loadObjFile(const std::string &fileName, Vector2 tiling = Vector2(1, 1));

#endif