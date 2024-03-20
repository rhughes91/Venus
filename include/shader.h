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
        uint32_t compileShader(const std::string &contents, uint32_t type) const;
};

// shader (namespace): global methods for loading and accessing shader data from .hlsl files
namespace shader
{
    void load(const std::string& path, const Shader& shader);
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

struct MeshModule
{
    Transform transform;
    Vector2 uvOffset = 0, uvScale = 1;
    Color hue = color::WHITE;

    MeshModule() {}
    MeshModule(const Transform& transform__) : transform(transform__), uvOffset(0), uvScale(1) {}
    MeshModule(const Transform& transform__, const Vector2& uvOffset__, const Vector2& uvScale__ = 1): transform(transform__), uvOffset(uvOffset__), uvScale(uvScale__) {}};

// Mesh (struct): wrapper for the Vertex Array Object and Vertex Buffer Object necessary to render a mesh
struct Mesh
{
    std::vector<Vertex> vertices;
    uint32_t VAO, VBO;
    Vector3 dimensions;

    Mesh() {}
    Mesh(const std::vector<Vector3> &vertices__, const std::vector<float> &texture__, const Vector3& dimensions__)
    {
        reinit(vertices__, texture__, dimensions__);
    }
    Mesh(const std::vector<Vertex> &vertices__, const Vector3& dimensions__) : vertices(vertices__), dimensions(dimensions__) {}

    void append(const Transform& parentTransform, const std::vector<MeshModule>& additions);
    void generate();
    void reinit(const std::vector<Vector3> &vertices__, const std::vector<float> &texture__, const Vector3& dimensions__);
    void refresh();
    void draw(const uint32_t texture) const;
    void remove();
};

struct MeshAddon
{
    std::vector<MeshModule> additions;

    MeshAddon() {}
    MeshAddon(const std::vector<MeshModule>& additions__) : additions(additions__) {}
};

// shape (namespace): provides basic Mesh shapes without needing to load a file
namespace shape
{
    Mesh sphere(float radius = 1, int32_t lod = 4);
    Mesh square(int32_t tiling = 1);
    Mesh cube();

    std::vector<Vector3> triangulate(const std::vector<Vector3>& vertices, int32_t recursions);
}

// mesh (namespace): global methods for loading and accessing mesh data from .obj files
namespace mesh
{
    Mesh &load(const std::string& path);
    Mesh &load(const std::string& path, const Mesh& mesh);
    void load(const std::string& path, const std::vector<std::string>& subPaths, const std::string& type);
    void set(const std::string& path, const Mesh& mesh);
    Mesh &get(const std::string& path);
    std::vector<Mesh> get(const std::string& path, const std::vector<std::string>& subPaths, const std::string& type);

    void remove();
    bool contains(const std::string& path);
};

namespace file
{
    // loads .obj file at 'filename' :: applied textures will be applied in a checkerboard pattern of size 'tiling'
    Mesh loadObjFile(const std::string &fileName);
}

class Model;
class Sprite;

struct SimpleShader
{
    Color color = color::WHITE;
    Vector2 offset, scale = 1;

    bool flip = false;

    SimpleShader(const Color& objColor = color::WHITE, const Vector2& uvOffset = 0, const Vector2& uvScale = 1) : color(objColor), offset(uvOffset), scale(uvScale), flip(false) {}
};
struct AdvancedShader : SimpleShader
{
    float ambient, diffuse, specular = 0;
    int32_t shine = 0;

    AdvancedShader(const Color& objColor = color::WHITE, float ambientStrength = 0, float diffuseStrength = 0, float specularStrength = 0, int32_t shininess = 0) : SimpleShader(objColor), ambient(ambientStrength), diffuse(diffuseStrength), specular(specularStrength), shine(shininess) {}
    AdvancedShader(const Color& objColor = color::WHITE, const Vector2& uvOffset = 0, const Vector2& uvScale = 1, float ambientStrength = 0, float diffuseStrength = 0, float specularStrength = 0, int32_t shininess = 0) : SimpleShader(objColor, uvOffset, uvScale), ambient(ambientStrength), diffuse(diffuseStrength), specular(specularStrength), shine(shininess) {}
};

//
namespace shader
{
    void simple(Shader& shader, const Transform& transform, const Camera& camera, const Transform& cameraTransform, const SimpleShader& mat);
    void ui(uint32_t entity, const Sprite& sprite, const Camera& camera, const Transform& cameraTransform);
    void advanced(Shader& shader, const Transform& transform, const Camera& camera, const Transform& cameraTransform, const AdvancedShader& mat);
}

// Model (struct): holds an entity's Material and Mesh data which allows it to be rendered
struct Model
{
    Mesh data;
    Texture texture;

    Model() {}
    Model(const Texture& texture__, Mesh data__ = mesh::get("square")) : data(data__), texture(texture__) {}

    void append(const Transform& parentTransform, const std::vector<MeshModule>& additions)
    {
        data.append(parentTransform, additions);
    }
    void refresh()
    {
        data.refresh();
    }
    
    void render()
    {
        data.draw(texture.texture);
    }
};

struct Sprite
{
    Texture texture;
    Color color;
    Shader shader;
    Vector2 offset, scale = 1;

    bool updateSorting = false, flip = false;

    // void(*run)(Entity entity, const Sprite& sprite, const Camera& camera, const Transform& cameraTransform, bool shaderMatch);

    Sprite(){}
    Sprite(const Texture& texture__, const Color& color__ = color::WHITE, const Shader& shader__ = shader::get("simple_shader")) : texture(texture__), color(color__), shader(shader__)
    {
        square = mesh::get("square");
    }
    
    void append(const Transform& parentTransform, const std::vector<MeshModule>& additions)
    {
        square.append(parentTransform, additions);
    }
    void refresh()
    {
        square.refresh();
    }
    void render()
    {
        square.draw(texture.texture);
    }

    void setSorting(float sorting__)
    {
        updateSorting = (sorting != sorting__);
        sorting = sorting__;
    }
    float getSorting()
    {
        return sorting;
    }

    Mesh square;
    float sorting = 0;
};

#endif