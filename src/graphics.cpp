#include "file_util.h"
#include "graphics.h"

#include "glad/glad.h"

uint32_t buffer::defaultType()
{
    return GL_FRAMEBUFFER;
}
uint32_t buffer::readType()
{
    return GL_READ_FRAMEBUFFER;
}
uint32_t buffer::drawType()
{
    return GL_DRAW_FRAMEBUFFER;
}
void buffer::enableDepthTest()
{
    glEnable(GL_DEPTH_TEST);
}
void buffer::disableDepthTest()
{
    glDisable(GL_DEPTH_TEST);
}
void buffer::blit(FrameBuffer& one, FrameBuffer& two, const Vector2& dim)
{
    one.bind(buffer::readType());
    two.bind(buffer::drawType());
    glBlitFramebuffer(0, 0, dim.x, dim.y, 0, 0, dim.x, dim.y, GL_COLOR_BUFFER_BIT, GL_NEAREST);
    one.unbind(buffer::readType());
    two.unbind(buffer::drawType());
}

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
    bind(GL_FRAMEBUFFER);
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
    unbind(GL_FRAMEBUFFER);
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
std::vector<uint8_t> FrameBuffer::getTextureData(const std::string& name)
{
    std::vector<uint8_t> result;
    TextureBuffer buffer = textures[name];
    if(buffer.type != GL_TEXTURE_2D)
        return result;
    
    int32_t textureWidth, textureHeight;
    bindTexture(name);
    glGetTexLevelParameteriv(GL_TEXTURE_2D, 0, GL_TEXTURE_WIDTH, &textureWidth);
    glGetTexLevelParameteriv(GL_TEXTURE_2D, 0, GL_TEXTURE_HEIGHT, &textureHeight);

    result.reserve(textureWidth * textureHeight * 4);
    glGetTexImage(GL_TEXTURE_2D, 0, GL_RGBA, GL_UNSIGNED_BYTE, &result[0]);

    return result;
}

