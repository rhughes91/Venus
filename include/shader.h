#pragma once

#include "color.h"
#include "serialize.h"
#include "vector.h"

#include <unordered_map>

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
    Vector3 offset, dimensions;


    static size_t length(const Mesh& data)
    {        
        return 
            object::length(data.vertices) + 
            object::length(data.VAO) + 
            object::length(data.VBO) + 
            object::length(data.dimensions) +
            object::length(data.offset);
    }

    static size_t serialize(const Mesh& value, std::vector<uint8_t>& stream, size_t index)
    {
        size_t count = 0;

        count += object::serialize(value.vertices, stream, index + count);
        count += object::serialize(value.VAO, stream, index + count);
        count += object::serialize(value.VBO, stream, index + count);
        count += object::serialize(value.dimensions, stream, index + count);
        count += object::serialize(value.offset, stream, index + count);

        return count;
    }

    static Mesh deserialize(std::vector<uint8_t>& stream, size_t index)
    {
        Mesh result = Mesh();
        size_t count = 0;

        result.vertices = object::deserialize<std::vector<Vertex>>(stream, index + count);
        count += object::length(result.vertices);

        result.VAO = object::deserialize<uint32_t>(stream, index + count);
        count += object::length(result.VAO);

        result.VBO = object::deserialize<uint32_t>(stream, index + count);
        count += object::length(result.VBO);

        result.dimensions = object::deserialize<Vector3>(stream, index + count);
        count += object::length(result.dimensions);

        result.offset = object::deserialize<Vector3>(stream, index + count);
        count += object::length(result.offset);

        return result;
    }


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
    void append(const std::vector<Vertex> &buffer, const Vector3& position, const Quaternion &rotation, const Quaternion &parentRotation, const Vector2& uvScale, const Vector2& uvOffset)
    {
        for(auto vertex : buffer)
        {
            vertex.position = (mat4x4)rotation * (mat4x4)parentRotation * vertex.position;
            vertex.position += position;
            vertex.position = (mat4x4)parentRotation.conjugate() * vertex.position;
            vertex.uv = vertex.uv * uvScale + uvOffset;
            vertices.push_back(vertex);
        }
    }
    std::string identifier() const
    {
        return "Mesh:"+std::to_string(VAO)+":"+std::to_string(VBO);
    }

    std::vector<Vertex> getVertices()
    {
        return vertices;
    }

    static Mesh &load(const std::string &path);
    static Mesh &load(const std::string& path, const Mesh& mesh);
    static void load(const std::string &path, const std::vector<std::string> &subPaths, const std::string &type);
    static void set(const std::string& path, const Mesh& mesh);
    static Mesh &get(const std::string &path);
    static std::vector<Mesh> get(const std::string &path, const std::vector<std::string> &subPaths, const std::string &type);
    static void clear();
    static bool contains(const std::string& path);

    private:
        std::vector<Vertex> vertices;
        uint32_t VAO, VBO;
        
        inline static std::unordered_map<std::string, Mesh> loadedMeshes;
};

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

    bool operator==(const Texture& comparison) const
    {
        return texture == comparison.texture && resolution == comparison.resolution; 
    }

    bool operator!=(const Texture& comparison) const
    {
        return !((*this) == comparison); 
    }

    private:
        inline static std::unordered_map<std::string, Texture> loadedTextures;
};

//
struct Animation2D
{
    std::vector<Texture> frames;
    uint32_t currentFrame = 0;

    Animation2D(std::vector<Texture> frames__ = std::vector<Texture>())
    {
        frames = frames__;
    }

    Texture current()
    {
        if(frames.size() == 0)
            return Texture();
        return frames[currentFrame];
    }
};

//
struct AnimationUV
{
    enum Type
    {
        LOOP, STOP
    };

    Vector2I bounds;
    Texture texture;
    uint32_t length, currentFrame = 0, rate = 0;
    Type type;

    AnimationUV(Texture texture__ = Texture(), uint32_t length__ = 0, uint32_t rate__ = 10, Type type__ = LOOP)
    {
        texture = texture__;
        length = length__;
        rate = rate__;
        type = type__;

        uint32_t square = std::ceil(std::sqrt(length));
        bool offset = (square * square - length) >= square; 
        bounds = Vector2I(square, square - (uint32_t)offset);
    }

    bool update()
    {
        frameCount = (frameCount + 1) % rate;
        return !frameCount;
    }

    private:
        uint32_t frameCount = 0;
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