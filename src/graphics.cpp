#include <iostream>
#include <string>
#include "setup.h"
#include "graphics.h"

extern Window g_window;
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

void FrameBuffer::refresh(uint16_t width, uint16_t height)
{
    for(auto texture : textures)
    {
        glBindTexture(texture.second.type, texture.second.data);
        if(texture.second.type == GL_TEXTURE_2D_MULTISAMPLE)
        {
            glTexImage2DMultisample(GL_TEXTURE_2D_MULTISAMPLE, 4, GL_RGB, width, height, GL_TRUE);
        }
        else
        {
            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
        }
    }
    for(auto renderBuffer : renderBuffers)
    {
        glBindRenderbuffer(GL_RENDERBUFFER, renderBuffer.second);
        glRenderbufferStorageMultisample(GL_RENDERBUFFER, 4, GL_DEPTH24_STENCIL8, width, height);
    }
}

bool FrameBuffer::complete()
{
    bind(GL_FRAMEBUFFER);
    bool complete = glCheckFramebufferStatus(GL_FRAMEBUFFER) == GL_FRAMEBUFFER_COMPLETE;
    unbind();
    return complete;
}

void FrameBuffer::addTexture(const std::string& name, uint16_t width, uint16_t height, uint32_t component, uint32_t componentType, uint32_t attachment, uint32_t scaling, uint32_t wrapping, bool multisampled)
{
    uint32_t texture;
    GLenum type = multisampled ? GL_TEXTURE_2D_MULTISAMPLE:GL_TEXTURE_2D;
    bind(GL_FRAMEBUFFER);
    glGenTextures(1, &texture);
    glBindTexture(type, texture);
    if(multisampled)
    {
        glTexImage2DMultisample(type, 4, component, width, height, GL_TRUE);
    }
    else
    {
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, scaling);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, scaling);
        glTexImage2D(type, 0, component, width, height, 0, component, GL_UNSIGNED_BYTE, NULL);      
    }
    
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, wrapping);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, wrapping);
    glFramebufferTexture2D(GL_FRAMEBUFFER, attachment, type, texture, 0);

    if(component != GL_RGB)
    {
        glDrawBuffer(GL_NONE);
        glReadBuffer(GL_NONE);
    }
    
    textures.insert({name, TextureBuffer{texture, type}});
    unbind();
}

void FrameBuffer::addRenderBuffer(const std::string& name)
{
    uint32_t renderBuffer;
    bind(GL_FRAMEBUFFER);

    glGenRenderbuffers(1, &renderBuffer);
    glBindRenderbuffer(GL_RENDERBUFFER, renderBuffer);
    glRenderbufferStorageMultisample(GL_RENDERBUFFER, 4, GL_DEPTH24_STENCIL8, g_window.SCR_WIDTH, g_window.SCR_HEIGHT);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, renderBuffer);
    renderBuffers.insert({name, renderBuffer});
    unbind();
}

void FrameBuffer::bind(uint32_t type)
{
    glBindFramebuffer(type, data);
}

void FrameBuffer::unbind()
{
    glBindFramebuffer(GL_FRAMEBUFFER, 0);  
}

void FrameBuffer::bindTexture(const std::string& name)
{
    glBindTexture(textures[name].type, textures[name].data);
}


Mesh::Mesh(Vector3 vertices__[], uint32_t numVertices, float texture__[], const Vector3& dimensions__) : count(numVertices), dimensions(dimensions__)
{
    std::vector<Vector3> positions = std::vector<Vector3>(count);
    for(int i=0; i<count; i++)
    {
        positions[i] = vertices__[i];
    }
    std::vector<float> textureCoords = std::vector<float>(2 * count);
    for(int i=0; i<count; i++)
    {
        textureCoords[i*2] = texture__[i*2];
        textureCoords[i*2 + 1] = texture__[i*2 + 1];
    }
    std::vector<Vertex> vertices;
    for(int i=0; i<count; i+=3)
    {
        Vector3 normal = vec3::triSurface(positions[i], positions[i+1], positions[i+2]);
        for(int j=0; j<3; j++)
        {
            vertices.push_back({positions[i+j], normal, Vector2(textureCoords[2*(i+j)], textureCoords[2*(i+j)+1])});
        }
    }
    glGenVertexArrays(1, &VAO);
    glBindVertexArray(VAO);

    glGenBuffers(1, &VBO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);

    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(Vertex), &vertices[0], GL_STATIC_DRAW);  

    // vertex positions
    glEnableVertexAttribArray(0);	
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)0);
    // vertex normals
    glEnableVertexAttribArray(1);	
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)sizeof(Vector3));
    // vertex texture coords
    glEnableVertexAttribArray(2);	
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)(sizeof(Vector3)*2));
}

Mesh::Mesh(const std::vector<Vertex> &vertices, const Vector3& dimensions__) : count(vertices.size()), dimensions(dimensions__)
{
    glGenVertexArrays(1, &VAO);
    glBindVertexArray(VAO);

    glGenBuffers(1, &VBO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);

    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(Vertex), &vertices[0], GL_STATIC_DRAW);  

    // vertex positions
    glEnableVertexAttribArray(0);	
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)0);
    // vertex normals
    glEnableVertexAttribArray(1);	
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)sizeof(Vector3));
    // vertex texture coords
    glEnableVertexAttribArray(2);	
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)(sizeof(Vector3)*2));
}

void Mesh::draw(const uint32_t texture) const
{
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, texture);

    glActiveTexture(GL_TEXTURE2);
    glBindTexture(GL_TEXTURE_2D, g_window.screen.depthBuffer.getTexture("texture").data);

    glBindVertexArray(VAO);
    glDrawArrays(GL_TRIANGLES, 0, count);
}

void Mesh::remove()
{
    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
}

Mesh shape::square(int32_t tiling)
{
    Vector3 vectors[]
    {
        Vector3(0.5f, -0.5f, 0),
        Vector3(-0.5f, 0.5f, 0),
        Vector3(-0.5f, -0.5f, 0),
        
        
        Vector3(0.5f, 0.5f, 0),
        Vector3(-0.5f, 0.5f, 0),
        Vector3(0.5f, -0.5f, 0),
        
    };
    float textureCorners = tiling;
    float texture[]
    {
        textureCorners, 0,
        0, textureCorners,
        0, 0,
        

        textureCorners, textureCorners,
        0, textureCorners,
        textureCorners, 0,
        
    };
    Mesh result = Mesh(vectors, 6, texture, Vector3(1, 1, 0));
    return result;
}

Mesh shape::cube()
{
    Vector3 vectors[]
    {
        Vector3(-0.5f, -0.5f, -0.5f),
        Vector3(0.5f, 0.5f, -0.5f),
        Vector3(0.5f, -0.5f, -0.5f),
        // NORTH
        Vector3(0.5f, 0.5f, -0.5f),
        Vector3(-0.5f, -0.5f, -0.5f),
        Vector3(-0.5f, 0.5f, -0.5f),

        Vector3(-0.5f, -0.5f, 0.5f),
        Vector3(0.5f, -0.5f, 0.5f),
        Vector3(0.5f, 0.5f, 0.5f),
        // SOUTH
        Vector3(0.5f, 0.5f, 0.5f),
        Vector3(-0.5f, 0.5f, 0.5f),
        Vector3(-0.5f, -0.5f, 0.5f),

        Vector3(-0.5f, 0.5f, 0.5f),
        Vector3(-0.5f, 0.5f, -0.5f),
        Vector3(-0.5f, -0.5f, -0.5f),
        // EAST
        Vector3(-0.5f, -0.5f, -0.5f),
        Vector3(-0.5f, -0.5f, 0.5f),
        Vector3(-0.5f, 0.5f, 0.5f),

        Vector3(0.5f, 0.5f, 0.5f),
        Vector3(0.5f, -0.5f, -0.5f),
        Vector3(0.5f, 0.5f, -0.5f),
        // WEST
        Vector3(0.5f, -0.5f, -0.5f),
        Vector3(0.5f, 0.5f, 0.5f),
        Vector3(0.5f, -0.5f, 0.5f),
        

        Vector3(-0.5f, -0.5f, -0.5f),
        Vector3(0.5f, -0.5f, -0.5f),
        Vector3(0.5f, -0.5f, 0.5f),
        // TOP
        Vector3(0.5f, -0.5f, 0.5f),
        Vector3(-0.5f, -0.5f, 0.5f),
        Vector3(-0.5f, -0.5f, -0.5f),

        Vector3(-0.5f, 0.5f, -0.5f),
        Vector3(0.5f, 0.5f, 0.5f),
        Vector3(0.5f, 0.5f, -0.5f),
        // BOTTOM
        Vector3(0.5f, 0.5f, 0.5f),
        Vector3(-0.5f, 0.5f, -0.5f),
        Vector3(-0.5f, 0.5f, 0.5f),
        
    };
    
    float texture[]
    {
        0, 0,
        1, 1,
        1, 0,
        // NORTH
        1, 1,
        0, 0,
        0, 1,

        0, 0,
        1, 0,
        1, 1,
        // SOUTH
        1, 1,
        0, 1,
        0, 0,

        1, 0,
        1, 1,
        0, 1,
        // EAST
        0, 1,
        0, 0,
        1, 0,

        1, 0,
        0, 1,
        1, 1,
        // WEST
        0, 1,
        1, 0,
        0, 0,
        

        0, 1,
        1, 1,
        1, 0,
        // TOP
        1, 0,
        0, 0,
        0, 1,

        0, 1,
        1, 0,
        1, 1,
        // BOTTOM
        1, 0,
        0, 1,
        0, 0,
        
    };
    Mesh result = Mesh(vectors, 36, texture, vec3::one);
    return result;
}
