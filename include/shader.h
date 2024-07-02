#ifndef SHADER_H
#define SHADER_H

#include "color.h"
#include "vector.h"

#include <vector>
#include <unordered_map>

template<typename T>
struct ShaderVar
{
    std::string name = "";
    T value;

    ShaderVar(const std::string& name__, const T& value__) : name(name__), value(value__) {}
};

// shader (struct): wrapper for graphical shader data :: allows for .hlsl files to be updated and used
struct Shader
{
    uint32_t ID;

    Shader() {ID = -1;}
    Shader(std::string vertexPath, std::string fragmentPath);
        
    void initialize(int32_t numPoints, int32_t numSpots);
    void refresh(int32_t numPoints, int32_t numSpots);
    void use() const;
    void remove() const;
    void setUniforms(const std::vector<std::string>& names, std::vector<int *>& results) const;
    
    void setBool(const std::string &name, bool value) const;  
    void setInt(const std::string &name, int32_t value) const;   
    void setFloat(const std::string &name, float value) const;
    void setVec2(const std::string &name, const Vector2 &vec2) const;
    void setVec3(const std::string &name, const Vector3 &vec3) const;
    void setColor3(const std::string &name, const Color &vec3) const;
    void setVec4(const std::string &name, const Color &vec4) const;
    void setMat4(const std::string &name, const float *, bool) const;

    void setBool(int loc, bool value) const;  
    void setInt(int loc, int32_t value) const;   
    void setFloat(int loc, float value) const;
    void setVec2(int loc, const Vector2 &vec2) const;
    void setVec3(int loc, const Vector3 &vec3) const;
    void setColor3(int loc, const Color &vec3) const;
    void setVec4(int loc, const Color &vec4) const;
    void setMat4(int loc, const float *, bool) const;

    static void load(const std::string& path, const Shader& shader);
    static Shader &get(const std::string& path);
    static void clear();

    private:
        uint32_t compileShader(const std::string &contents, uint32_t type) const;
        inline static std::unordered_map<std::string, Shader> loadedShaders;
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
    Vector3 offset, dimensions;

    Mesh() {}
    Mesh(const std::vector<Vector3> &vertices__, const std::vector<float> &texture__, const Vector3& dimensions__, const Vector3& offset__ = 0)
    {
        reinit(vertices__, texture__, dimensions__);
    }
    Mesh(const std::vector<Vertex> &vertices__, const Vector3& dimensions__, const Vector3& offset__ = 0) : vertices(vertices__), dimensions(dimensions__), offset(offset__) {}

    void generate();
    void reinit(const std::vector<Vector3> &vertices__, const std::vector<float> &texture__, const Vector3& dimensions__);
    void refresh();
    void draw(const uint32_t texture) const;
    void remove();

    static Mesh &load(const std::string &path);
    static Mesh &load(const std::string& path, const Mesh& mesh);
    static void load(const std::string &path, const std::vector<std::string> &subPaths, const std::string &type);
    static void set(const std::string& path, const Mesh& mesh);
    static Mesh &get(const std::string &path);
    static std::vector<Mesh> get(const std::string &path, const std::vector<std::string> &subPaths, const std::string &type);
    static void clear();
    static bool contains(const std::string& path);

    private:
        inline static std::unordered_map<std::string, Mesh> loadedMeshes;
};

// namespace texture
// {
    
// };

//
struct Texture
{
    enum Channel
    {
        RED, RGB, RGBA
    };
    static uint32_t channelToModifier(Channel channel);

    enum Type
    {
        PNG, JPEG
    };
    inline static std::string typeToString(Type type)
    {
        switch(type)
        {
            case PNG: return "png";
            case JPEG: return "jpeg";
        }
        return "";
    }
    static uint32_t typeToModifier(Type type);

    enum Filter
    {
        POINT, LINEAR
    };
    static uint32_t filterToModifier(Filter filter);


    uint32_t texture;
    Vector2I resolution;

    Texture() {}
    Texture(uint32_t texture__, const Vector2I& resolution__) : texture(texture__), resolution(resolution__) {}

    static void load(const std::string &path, Type type);
    static void load(const std::string &path, const std::vector<std::string> &subPaths, Type type);
    static void load(const std::string& name, const std::vector<char>& data, float width, float height, Channel channel, Type type);
    static void load(const std::string& name, const std::vector<Color8>& data, float width, float height, Channel channel, Type type, Filter filter = Filter::LINEAR);
    static Texture loadTo(const std::vector<Color8>& data, float width, float height, Channel channel, Type type, Filter filter);
    static Texture get(const std::string &path);
    static std::vector<Texture> get(const std::string &path, const std::vector<std::string> &subPaths, Type type);
    static void clear();

    private:
        inline static std::unordered_map<std::string, Texture> loadedTextures;
};

//
struct Animation2D
{
    std::vector<Texture> frames;
};

// file (namespace)
namespace file
{
    // loads .obj file at 'filename' :: applied textures will be applied in a checkerboard pattern of size 'tiling'
    Mesh loadObjFile(const std::string &fileName);

    std::vector<char> loadPNG(const std::string &fileName);
}

// shape (namespace): provides basic Mesh shapes without needing to load a file
namespace shape
{
    Mesh sphere(float radius = 1, int32_t lod = 4);
    Mesh square(int32_t tiling = 1);
    Mesh cube();

    std::vector<Vector3> triangulate(const std::vector<Vector3>& vertices, int32_t recursions);
}

#endif