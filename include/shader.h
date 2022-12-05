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
    void setVec3(const std::string &name, const Color &vec3) const;
    void setVec4(const std::string &name, const Color &vec4) const;
    void setMat4(const std::string &name, const float *, bool) const;

    private:
        std::string m_vertexPath, m_fragmentPath;
        uint32_t compileShader(const std::string &contents, uint32_t type) const;
};

// shader (namespace): global methods for loading and accessing shader data from .hlsl files
namespace shader
{
    Shader &set(const std::string& path, const Shader& shader);
    Shader &get(const std::string& path);
    void remove();

    void simple(const Transform& transform, const Model& model, const Camera& camera, const Transform& cameraTransform);
    void advanced(const Transform& transform, const Model& model, const Camera& camera, const Transform& cameraTransform, const Vector3& ambient, const Vector3& diffuse, const Vector3& specular, int32_t shininess);
};

// Material (struct): holds the information necessary to render an entity to a complex Shader
struct Material
{
    Shader shader;

    Vector3 ambientStrength, diffuseStrength, specularStrength;
    Color specular;
    float shininess;

    // determines whether the object responds to point and spot lights
    bool useAdvancedLighting = true;
    void(*run)(const Transform& transform, const Model& model, const Camera& camera, const Transform& cameraTransform);

    Material(const Shader &shader__ = Shader()) : shader(shader__)
    {
        run = shader::simple;
    }

    Material(const Shader& shader__, void(*runFunction)(const Transform& transform, const Model& model, const Camera& camera, const Transform& cameraTransform)) : shader(shader__)
    {
        run = runFunction;
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


// Model (struct): holds an entity's Material and Mesh data which allows it to be rendered
struct Model
{
    Color color;
    Material material;
    Mesh data;
    uint32_t texture;

    Model(const Color& color__ = color::WHITE, const Material& material__ = Material(), const Mesh &data__ = Mesh(), uint32_t texture__ = 0) : color(color__), material(material__), data(data__), texture(texture__) {}

    // uses the Material's shader of this Model
    void draw()
    {
        data.draw(texture);
    }
};

// loads .obj file at 'filename' :: applied textures will be applied in a checkerboard pattern of size 'tiling'
Mesh loadObjFile(const std::string &fileName, Vector2 tiling = Vector2(1, 1));

#endif