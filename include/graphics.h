#ifndef GRAPHICS_H
#define GRAPHICS_H

#include "shader.h"

#include <string>
#include <unordered_map>

// TextureBuffer (struct): wrapper for graphical texture data
struct TextureBuffer
{
    uint32_t data, type, component;
};

// FrameBuffer (struct): wrapper for graphical framebuffer data :: allows for simpler editing of the framebuffer data
struct FrameBuffer
{
    uint32_t data;
    std::unordered_map<std::string, uint32_t> renderBuffers;
    std::unordered_map<std::string, TextureBuffer> textures;

    void initialize();
    void remove();
    void refresh(uint16_t width, uint16_t height, bool opaque);
    int complete();

    void addTexture(const std::string& name, uint16_t width, uint16_t height, uint32_t component, uint32_t componentType, uint32_t attachment, uint32_t scaling, uint32_t wrapping, int samples);
    void addRenderBuffer(const std::string& name, uint16_t width, uint16_t height, int samples);

    void bind(uint32_t type);
    void unbind(uint32_t type);
    void bindTexture(const std::string& name);
    TextureBuffer getTexture(const std::string& name)
    {
        return textures[name];
    }
    std::vector<uint8_t> getTextureData(const std::string& name);
};

//
struct SimpleShader
{
    Color color = color::WHITE;

    bool flip = false;

    SimpleShader(const Color& objColor = color::WHITE, const Vector2& uvOffset = 0) : color(objColor), flip(false) {}
};

//
struct AdvancedShader : SimpleShader
{
    float ambient, diffuse, specular = 0;
    int32_t shine = 0;

    AdvancedShader(const Color& objColor = color::WHITE, float ambientStrength = 0, float diffuseStrength = 0, float specularStrength = 0, int32_t shininess = 0) : SimpleShader(objColor), ambient(ambientStrength), diffuse(diffuseStrength), specular(specularStrength), shine(shininess) {}
};

//
struct ComplexShader : SimpleShader
{
    ComplexShader(const Color& objColor = color::WHITE) : SimpleShader(objColor) {}
};

//
struct TextShader : SimpleShader
{

};

//
struct Fade
{
    float rate, distance;

    Fade(float newRate = 0, float newDistance = 0) : rate(newRate), distance(newDistance) {}
};

// Model (struct): holds an entity's Texture and Mesh data which allows it to be rendered
struct Model
{
    Mesh data;
    Texture texture;
    Vector2 offset, scale = 1;

    Model() {}
    Model(const Texture& texture__, Vector2 offset__, Vector2 scale__, Mesh data__ = Mesh::get("square")) : data(data__), offset(offset__), scale(scale__), texture(texture__) {}
    Model(const Texture& texture__, Mesh data__ = Mesh::get("square")) : data(data__), texture(texture__) {}

    void refresh()
    {
        data.refresh();
    }
    
    void render()
    {
        data.draw(texture.texture);
    }

    bool in(const Frustum& frustum)
    {
        return false;
    }
};


// buffer (namespace)
namespace buffer
{
    uint32_t defaultType();
    uint32_t readType();
    uint32_t drawType();

    void enableDepthTest();
    void disableDepthTest();
    void blit(FrameBuffer& one, FrameBuffer& two, const Vector2& dim);
};


#endif