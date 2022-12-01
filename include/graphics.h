#ifndef GRAPHICS_H
#define GRAPHICS_H

#include <vector>
#include <unordered_map>

#include "vector.h"

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

// texture (namespace): global methods for loading and accessing image data from image files
namespace texture
{
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

    void set(const std::string& path, int32_t screenChannel);
    void set(const std::string& path, const std::vector<std::string>& subPaths, Type type);
    uint32_t get(const std::string& path);
    std::vector<uint32_t> get(const std::string& path, const std::vector<std::string>& subPaths, Type type);

    void remove();
};

#endif