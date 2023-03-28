#include "glad/glad.h"
#include "image/stb_image.h"

#include "graphics.h"
#include "structure.h"

extern Time g_time;

void FrameBuffer::initialize()
{
    glGenFramebuffers(1, &data);
}
void FrameBuffer::remove()
{
    for(auto buffer : renderBuffers)
    {
        glDeleteRenderbuffers(1, &(buffer.second));
    }
    for(auto texture : textures)
    {
        glDeleteTextures(1, &(texture.second.data));
    }
    glDeleteFramebuffers(1, &data);
}
void FrameBuffer::refresh(uint16_t width, uint16_t height, bool opaque)
{
    for(auto texture : textures)
    {
        glBindTexture(texture.second.type, texture.second.data);
        if(texture.second.type == GL_TEXTURE_2D_MULTISAMPLE)
        {
            texture.second.component = opaque ? GL_RGB : GL_RGBA;
            glTexImage2DMultisample(GL_TEXTURE_2D_MULTISAMPLE, 4, texture.second.component, width, height, GL_TRUE);
        }
        else
        {
            glTexImage2D(GL_TEXTURE_2D, 0, texture.second.component, width, height, 0, texture.second.component, GL_UNSIGNED_BYTE, NULL);
        }
    }
    
    for(auto renderBuffer : renderBuffers)
    {
        glBindRenderbuffer(GL_RENDERBUFFER, renderBuffer.second);
        glRenderbufferStorageMultisample(GL_RENDERBUFFER, 4, GL_DEPTH24_STENCIL8, width, height);
    }
}
void FrameBuffer::addTexture(const std::string& name, uint16_t width, uint16_t height, uint32_t component, uint32_t componentType, uint32_t attachment, uint32_t scaling, uint32_t wrapping, int samples = 0)
{
    uint32_t texture;
    GLenum type = samples ? GL_TEXTURE_2D_MULTISAMPLE:GL_TEXTURE_2D;
    bind(GL_FRAMEBUFFER);

    glGenTextures(1, &texture);
    glBindTexture(type, texture);
    
    if(samples)
    {
        glTexImage2DMultisample(type, samples, component, width, height, GL_TRUE);
    }
    else
    {
        glTexImage2D(type, 0, component, width, height, 0, component, GL_UNSIGNED_BYTE, NULL);

        glTexParameteri(type, GL_TEXTURE_MIN_FILTER, scaling);
        glTexParameteri(type, GL_TEXTURE_MAG_FILTER, scaling);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, wrapping);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, wrapping);
    }
    glFramebufferTexture2D(GL_FRAMEBUFFER, attachment, type, texture, 0);
    
    textures.insert({name, TextureBuffer{texture, type, component}});
    unbind(GL_FRAMEBUFFER);
}
void FrameBuffer::addRenderBuffer(const std::string& name, uint16_t width, uint16_t height, int samples)
{
    uint32_t renderBuffer;
    bind(GL_FRAMEBUFFER);
    
    glGenRenderbuffers(1, &renderBuffer);
    glBindRenderbuffer(GL_RENDERBUFFER, renderBuffer);
    glRenderbufferStorageMultisample(GL_RENDERBUFFER, samples, GL_DEPTH24_STENCIL8, width, height);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, renderBuffer);
    renderBuffers.insert({name, renderBuffer});
    
    unbind(GL_FRAMEBUFFER);
}

void FrameBuffer::bind(uint32_t type)
{
    glBindFramebuffer(type, data);
}
void FrameBuffer::unbind(uint32_t type)
{
    glBindFramebuffer(type, 0);  
}
void FrameBuffer::bindTexture(const std::string& name)
{
    glBindTexture(textures[name].type, textures[name].data);
}
int FrameBuffer::complete()
{
    bind(GL_FRAMEBUFFER);
    int complete = glCheckFramebufferStatus(GL_FRAMEBUFFER);
    unbind(GL_FRAMEBUFFER);
    return complete;
}

extern std::unordered_map<std::string, uint32_t> g_loadedTextures;

uint32_t texture::channelToModifier(texture::Channel channel)
{
    switch(channel)
    {
        case texture::RED: return GL_RED;
        case texture::RGB: return GL_RGB;
        case texture::RGBA: return GL_RGBA;
    }
    return GL_RGB;
}
uint32_t texture::typeToModifier(texture::Type type)
{
    switch(type)
    {
        case PNG: return GL_SRGB_ALPHA;
        case JPEG: return GL_SRGB;
    }
    return GL_SRGB;
}

void texture::load(const std::string &path, texture::Type type)
{
    int32_t screenChannel = texture::typeToModifier(type);
    std::string screenChannelString = texture::typeToString(type);

    int32_t width, height, channels;
    unsigned char *data = stbi_load((source::root() + source::texture() + path + "." + screenChannelString).c_str(), &width, &height, &channels, 0);

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
    g_loadedTextures[path + "." + screenChannelString] = texture;
}
void texture::load(const std::string &path, const std::vector<std::string> &subPaths, texture::Type type)
{
    for (std::string subPath : subPaths)
    {
        texture::load(path + subPath, type);
    }
}
void texture::load(const std::string& name, const std::vector<char>& data, float width, float height, texture::Channel channel, texture::Type type)
{
    uint32_t imageChannel = texture::channelToModifier(channel);
    uint32_t screenChannel = texture::typeToModifier(type);

    uint32_t texture;
    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_2D, texture);
    glTexImage2D(GL_TEXTURE_2D, 0, screenChannel, width, height, 0, imageChannel, GL_UNSIGNED_BYTE, &data[0]);
    glGenerateMipmap(GL_TEXTURE_2D);

    // set the texture wrapping/filtering options (on the currently bound texture object)
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAX_ANISOTROPY, 16);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    g_loadedTextures[name] = texture;
}
void texture::load(const std::string& name, const std::vector<Color8>& data, float width, float height, texture::Channel channel, texture::Type type)
{
    uint32_t imageChannel = texture::channelToModifier(channel);
    uint32_t screenChannel = texture::typeToModifier(type);

    uint32_t texture;
    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_2D, texture);
    glTexImage2D(GL_TEXTURE_2D, 0, screenChannel, width, height, 0, imageChannel, GL_UNSIGNED_BYTE, &data[0]);
    glGenerateMipmap(GL_TEXTURE_2D);

    // set the texture wrapping/filtering options (on the currently bound texture object)
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAX_ANISOTROPY, 16);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    g_loadedTextures[name] = texture;
}

uint32_t texture::get(const std::string &path)
{
    if(!g_loadedTextures.count(path))
    {
        std::cout << "ERROR :: Texture at \'" << path << "\' could not be found." << std::endl;
        return g_loadedTextures.at("default");
    }
    return g_loadedTextures.at(path);
}
std::vector<uint32_t> texture::get(const std::string &path, const std::vector<std::string> &subPaths, texture::Type type)
{
    std::vector<uint32_t> textures;
    for (std::string subPath : subPaths)
    {
        std::string pathName = path + subPath + "." + texture::typeToString(type);
        if(!g_loadedTextures.count(pathName))
        {
            std::cout << "ERROR :: Texture at \'" << pathName << "\' could not be found." << std::endl;
            return std::vector<uint32_t>();
        }
        textures.push_back(g_loadedTextures[pathName]);
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