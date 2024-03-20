#ifndef GRAPHICS_H
#define GRAPHICS_H

#include <vector>
#include <unordered_map>

#include "vector.h"
#include "color.h"

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

namespace buffer
{
    uint32_t defaultType();
    uint32_t readType();
    uint32_t drawType();

    void enableDepthTest();
    void disableDepthTest();
    void blit(FrameBuffer& one, FrameBuffer& two);
};

struct Texture
{
    uint32_t texture;
    Vector2I resolution;

    Texture() {}
    Texture(uint32_t texture__, const Vector2I& resolution__) : texture(texture__), resolution(resolution__) {}
};

// texture (namespace): global methods for loading and accessing image data from image files
namespace texture
{
    enum Channel
    {
        RED, RGB, RGBA
    };
    uint32_t channelToModifier(Channel channel);

    enum Type
    {
        PNG, JPEG
    };
    inline std::string typeToString(Type type)
    {
        switch(type)
        {
            case PNG: return "png";
            case JPEG: return "jpeg";
        }
        return "";
    }
    uint32_t typeToModifier(Type type);

    enum Filter
    {
        POINT, LINEAR
    };
    uint32_t filterToModifier(Filter filter);

    void load(const std::string& path, Type type);
    void load(const std::string& path, const std::vector<std::string>& subPaths, Type type);
    void load(const std::string& name, const std::vector<char>& data, float width, float height, Channel channel, Type type);
    void load(const std::string& name, const std::vector<Color8>& data, float width, float height, Channel channel, Type type, Filter filter = LINEAR);
    
    Texture loadTo(const std::vector<Color8>& data, float width, float height, Channel channel, Type type, Filter filter = LINEAR);
    
    Texture get(const std::string& path);
    std::vector<Texture> get(const std::string& path, const std::vector<std::string>& subPaths, Type type);

    void remove();

    void writeRaw(const std::string& name, int32_t width, int32_t height, int32_t channels, uint8_t *data);
};

#endif