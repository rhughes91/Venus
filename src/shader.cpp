#include "file_util.h"
#include "shader.h"

#include "glad/glad.h"
#include "image/stb_image.h"

#include <iostream>
#include <string>
#include <unordered_map>


Shader::Shader(std::string vertexPath, std::string fragmentPath)
{
    ID = glCreateProgram();
    uint32_t shader;
    if(shader = compileShader
    (
        file::loadFileToString((Source::shader()+vertexPath+".hlsl").c_str()), GL_VERTEX_SHADER
    ))
    {
        glAttachShader(ID, shader);
        glDeleteShader(shader);
    }
    else return;
    if(shader = compileShader
    (
        file::loadFileToString((Source::shader()+fragmentPath+".hlsl").c_str()), GL_FRAGMENT_SHADER
    ))
    {
        glAttachShader(ID, shader);
        glDeleteShader(shader);
    }
    else return;

    GLint success = 0;
    glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
    if(!success)
    {
        std::cout << "ERROR :: Shader compilation failed." << std::endl;
        return;
    }

    glLinkProgram(ID);
}
void Shader::use() const
{
    glUseProgram(ID);
}
void Shader::remove() const
{
    glDeleteProgram(ID);
}
void Shader::setUniforms(const std::vector<std::string>& names, std::vector<int *>& results) const
{
    for(int i=0; i<names.size(); i++)
    {
        *results[i] = glGetUniformLocation(ID, names[i].c_str());
    }
}

void Shader::setBool(const std::string &name, bool value) const
{         
    glUniform1i(glGetUniformLocation(ID, name.c_str()), (int)value); 
}
void Shader::setInt(const std::string &name, int32_t value) const
{ 
    int transformLoc = glGetUniformLocation(ID, name.c_str());
    glUniform1i(transformLoc, value); 
}
void Shader::setFloat(const std::string &name, float value) const
{
    int transformLoc = glGetUniformLocation(ID, name.c_str());
    glUniform1f(transformLoc, value); 
} 
void Shader::setVec2(const std::string &name, const Vector2 &vec2) const
{
    int transformLoc = glGetUniformLocation(ID, name.c_str());
    // if(transformLoc == -1) std::cout << ID << " :: " << name << " = " << vec2 << std::endl;
    glUniform2f(transformLoc, vec2.x, vec2.y);
}
void Shader::setVec3(const std::string &name, const Vector3 &vec3) const
{
    int transformLoc = glGetUniformLocation(ID, name.c_str());
    // if(transformLoc == -1) std::cout << ID << " :: " << name << " = " << vec3 << std::endl;
    glUniform3f(transformLoc, vec3.x, vec3.y, vec3.z);
}
void Shader::setColor3(const std::string &name, const Color &vec3) const
{
    int transformLoc = glGetUniformLocation(ID, name.c_str());
    glUniform3f(transformLoc, vec3.r, vec3.g, vec3.b);
}
void Shader::setVec4(const std::string &name, const Color &vec4) const
{
    int transformLoc = glGetUniformLocation(ID, name.c_str());
    glUniform4f(transformLoc, vec4.r, vec4.g, vec4.b, vec4.a);
}
void Shader::setMat4(const std::string &name, const float *transform, bool transpose) const
{
    int transformLoc = glGetUniformLocation(ID, name.c_str());
    glUniformMatrix4fv(transformLoc, 1, transpose, transform);
}

void Shader::setBool(int loc, bool value) const
{         
    glUniform1i(loc, (int)value); 
}
void Shader::setInt(int loc, int32_t value) const
{ 
    glUniform1i(loc, value); 
}
void Shader::setFloat(int loc, float value) const
{
    glUniform1f(loc, value); 
} 
void Shader::setVec2(int loc, const Vector2 &vec2) const
{
    glUniform2f(loc, vec2.x, vec2.y);
}
void Shader::setVec3(int loc, const Vector3 &vec3) const
{
    glUniform3f(loc, vec3.x, vec3.y, vec3.z);
}
void Shader::setColor3(int loc, const Color &vec3) const
{
    glUniform3f(loc, vec3.r, vec3.g, vec3.b);
}
void Shader::setVec4(int loc, const Color &vec4) const
{
    glUniform4f(loc, vec4.r, vec4.g, vec4.b, vec4.a);
}
void Shader::setMat4(int loc, const float *transform, bool transpose) const
{
    glUniformMatrix4fv(loc, 1, transpose, transform);
}

uint32_t Shader::compileShader(const std::string &contents, uint32_t type) const
{
    const char *shaderSource = contents.c_str();

    uint32_t shader;
    shader = glCreateShader(type);
    glShaderSource(shader, 1, &shaderSource, NULL);
    glCompileShader(shader);

    int32_t success;
    char infoLog[512];
    glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
    if(!success)
    {
        glGetShaderInfoLog(shader, 512, NULL, infoLog);
        std::cout << "ERROR :: Shader compilated failed: " << infoLog << std::endl;
        return 0;
    }

    return shader;
}

void Shader::load(const std::string& path, const Shader& shader)
{
    loadedShaders[path] = shader;
}
Shader &Shader::get(const std::string& path)
{
    if(!loadedShaders.count(path))
    {
        std::cout << "ERROR :: Shader at \'" << path << "\' could not be found." << std::endl;
        // return mesh::get("square");
    }
    return loadedShaders.at(path);
}
void Shader::clear()
{
    for (auto &pair : loadedShaders)
    {
        pair.second.remove();
    }
}

void Mesh::draw(const uint32_t texture) const
{
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, texture);

    // glActiveTexture(GL_TEXTURE2);
    // glBindTexture(GL_TEXTURE_2D, g_windows[currentWindow].screen.depthBuffer.getTexture("texture").data);
    
    glBindVertexArray(VAO);
    glDrawArrays(GL_TRIANGLES, 0, vertices.size());
}
void Mesh::reinit(const std::vector<Vector3>& vertices__, const std::vector<float>& texture__, const Vector3& dimensions__)
{
    dimensions = dimensions__;
    vertices.clear();
    if(!vertices__.size() || !texture__.size())
        return;

    for(int i=0; i<vertices__.size(); i+=3)
    {
        Vector3 normal = vec3::triSurface(vertices__[i], vertices__[i+1], vertices__[i+2]);
        for(int j=0; j<3; j++)
        {
            vertices.push_back({vertices__[i+j], normal, Vector2(texture__[(2*(i+j)) % texture__.size()], texture__[(2*(i+j)+1) % texture__.size()])});
        }
    }
}
void Mesh::generate()
{
    glGenVertexArrays(1, &VAO);
    glBindVertexArray(VAO);
    glGenBuffers(1, &VBO);
}
void Mesh::refresh()
{
    glBindVertexArray(VAO);
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
void Mesh::remove()
{
    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
}

Mesh &Mesh::load(const std::string &path)
{
    loadedMeshes[path] = file::loadObjFile(path);
    loadedMeshes[path].generate();
    return(loadedMeshes[path]);
}
Mesh &Mesh::load(const std::string& path, const Mesh& mesh)
{
    loadedMeshes[path] = mesh;
    loadedMeshes[path].generate();
    return(loadedMeshes[path]);
}
void Mesh::load(const std::string &path, const std::vector<std::string> &subPaths, const std::string &type)
{
    for (std::string subPath : subPaths)
    {
        load(path + subPath + "." + type);
    }
}
void Mesh::set(const std::string& path, const Mesh& mesh)
{
    loadedMeshes[path] = mesh;
}
Mesh &Mesh::get(const std::string &path)
{
    if(!loadedMeshes.count(path))
    {
        std::cout << "ERROR :: Mesh at \'" << path << "\' could not be found." << std::endl;
        return get("square");
    }
    return loadedMeshes.at(path);
}
std::vector<Mesh> Mesh::get(const std::string &path, const std::vector<std::string> &subPaths, const std::string &type)
{
    std::vector<Mesh> meshes;
    for (std::string subPath : subPaths)
    {
        meshes.push_back(loadedMeshes[path + subPath + "." + type]);
    }
    return meshes;
}
void Mesh::clear()
{
    for (auto &pair : loadedMeshes)
    {
        pair.second.remove();
    }
}
bool Mesh::contains(const std::string& path)
{
    return loadedMeshes.count(path);
}

Mesh shape::sphere(float radius, int32_t lod)
{
    float value = 0.5f;
    float sqrt2 = std::sqrt(4*value)/2;
    std::vector<Vector3> vectors
    {
        // TOWARDS-TOP
        Vector3(-value, 0, -value),
        Vector3(0, sqrt2, 0),
        Vector3(value, 0, -value),

        // AWAY-TOP
        Vector3(value, 0, value),
        Vector3(0, sqrt2, 0),
        Vector3(-value, 0, value),
        
        // RIGHT-TOP
        Vector3(-value, 0, value),
        Vector3(0, sqrt2, 0),
        Vector3(-value, 0, -value),      

        // LEFT-TOP
        Vector3(value, 0, -value),
        Vector3(0, sqrt2, 0),
        Vector3(value, 0, value),

        // TOWARDS-BOTTOM
        Vector3(value, 0, -value),
        Vector3(0, -sqrt2, 0),
        Vector3(-value, 0, -value),

        // AWAY-BOTTOM
        Vector3(-value, 0, value),
        Vector3(0, -sqrt2, 0),
        Vector3(value, 0, value),
        
        // RIGHT-BOTTOM
        Vector3(-value, 0, -value),
        Vector3(0, -sqrt2, 0),
        Vector3(-value, 0, value),      

        // LEFT-BOTTOM
        Vector3(value, 0, value),
        Vector3(0, -sqrt2, 0),
        Vector3(value, 0, -value),
    };

    vectors = shape::triangulate(vectors, lod);
    for(auto& vector: vectors)
        vector = vector.normalized() * radius/2;

    std::vector<float> texture
    {
        0, 0,
        0, 1,
        1, 0,
    };
    Mesh result = Mesh(vectors, texture, radius);

    return result;
}
Mesh shape::square(int32_t tiling)
{
    std::vector<Vector3> vectors
    {
        Vector3(0.5f, -0.5f, 0),
        Vector3(-0.5f, 0.5f, 0),
        Vector3(-0.5f, -0.5f, 0),
        
        Vector3(0.5f, 0.5f, 0),
        Vector3(-0.5f, 0.5f, 0),
        Vector3(0.5f, -0.5f, 0),
    };
    std::vector<float> texture
    {
        (float)tiling, 0,
        0, (float)tiling,
        0, 0,
        (float)tiling, (float)tiling,
        0, (float)tiling,
        (float)tiling, 0,
    };
    Mesh result = Mesh(vectors, texture, Vector3(1, 1, 0));
    return result;
}
Mesh shape::cube()
{
    std::vector<Vector3> vectors
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
        // BOTTOM
        Vector3(0.5f, -0.5f, 0.5f),
        Vector3(-0.5f, -0.5f, 0.5f),
        Vector3(-0.5f, -0.5f, -0.5f),

        Vector3(-0.5f, 0.5f, -0.5f),
        Vector3(0.5f, 0.5f, 0.5f),
        Vector3(0.5f, 0.5f, -0.5f),
        // TOP
        Vector3(0.5f, 0.5f, 0.5f),
        Vector3(-0.5f, 0.5f, -0.5f),
        Vector3(-0.5f, 0.5f, 0.5f),
        
    };
    
    std::vector<float> texture
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
    Mesh result = Mesh(vectors, texture, vec3::one);
    return result;
}

std::vector<Vector3> shape::triangulate(const std::vector<Vector3>& vertices, int32_t recursions)
{
    std::vector<Vector3> result;
    for(int i=0; i<vertices.size(); i+=3)
    {
        result.push_back(vertices[i]);
        result.push_back((vertices[i] + vertices[i+1])/2);
        result.push_back((vertices[i] + vertices[i+2])/2);

        result.push_back((vertices[i+1] + vertices[i+2])/2);
        result.push_back((vertices[i] + vertices[i+2])/2);
        result.push_back((vertices[i] + vertices[i+1])/2);

        result.push_back((vertices[i] + vertices[i+2])/2);
        result.push_back((vertices[i+1] + vertices[i+2])/2);
        result.push_back(vertices[i+2]);

        result.push_back((vertices[i] + vertices[i+1])/2);
        result.push_back(vertices[i+1]);
        result.push_back((vertices[i+1] + vertices[i+2])/2);
    }
    if(recursions == 1)
        return result;

    return triangulate(result, recursions-1);
}

uint32_t Texture::channelToModifier(Channel channel)
{
    switch(channel)
    {
        case RED: return GL_RED;
        case RGB: return GL_RGB;
        case RGBA: return GL_RGBA;
    }
    return GL_RGB;
}
uint32_t Texture::typeToModifier(Type type)
{
    switch(type)
    {
        case PNG: return GL_SRGB_ALPHA;
        case JPEG: return GL_SRGB;
    }
    return GL_SRGB;
}
uint32_t Texture::filterToModifier(Filter filter)
{
    switch(filter)
    {
        case POINT: return GL_NEAREST;
        case LINEAR: return GL_LINEAR;
    }
    return -1;
}

void Texture::load(const std::string &path, Type type)
{
    int32_t screenChannel = typeToModifier(type);
    std::string screenChannelString = typeToString(type);

    int32_t width, height, channels;
    unsigned char *data = stbi_load((Source::root() + Source::texture() + path + "." + screenChannelString).c_str(), &width, &height, &channels, 0);

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
    loadedTextures[path + "." + screenChannelString] = Texture(texture, Vector2I(width, height));
}
void Texture::load(const std::string &path, const std::vector<std::string> &subPaths, Type type)
{
    for (std::string subPath : subPaths)
    {
        load(path + subPath, type);
    }
}
void Texture::load(const std::string& name, const std::vector<char>& data, float width, float height, Channel channel, Type type)
{
    uint32_t imageChannel = channelToModifier(channel);
    uint32_t screenChannel = typeToModifier(type);

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

    loadedTextures[name] = Texture(texture, Vector2I(width, height));
}
void Texture::load(const std::string& name, const std::vector<Color8>& data, float width, float height, Channel channel, Type type, Filter filter)
{
    uint32_t imageChannel = channelToModifier(channel);
    uint32_t screenChannel = typeToModifier(type);
    uint32_t filterChannel = filterToModifier(filter);
    uint32_t minFilter = -1;
    switch(filterChannel)
    {
        case GL_NEAREST: 
            minFilter = GL_NEAREST_MIPMAP_LINEAR;
        break;
        case GL_LINEAR:
            minFilter = GL_LINEAR_MIPMAP_LINEAR;
        break;
    }

    uint32_t texture;
    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_2D, texture);
    glTexImage2D(GL_TEXTURE_2D, 0, screenChannel, width, height, 0, imageChannel, GL_UNSIGNED_BYTE, &data[0]);
    glGenerateMipmap(GL_TEXTURE_2D);

    // set the texture wrapping/filtering options (on the currently bound texture object)
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAX_ANISOTROPY, 16);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, minFilter);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, filterChannel);

    loadedTextures[name] = Texture(texture, Vector2I(width, height));
}
Texture Texture::loadTo(const std::vector<Color8>& data, float width, float height, Channel channel, Type type, Filter filter)
{
    uint32_t imageChannel = channelToModifier(channel);
    uint32_t screenChannel = typeToModifier(type);
    uint32_t filterChannel = filterToModifier(filter);
    uint32_t minFilter = -1;
    switch(filterChannel)
    {
        case GL_NEAREST: 
            minFilter = GL_NEAREST_MIPMAP_LINEAR;
        break;
        case GL_LINEAR:
            minFilter = GL_LINEAR_MIPMAP_LINEAR;
        break;
    }

    uint32_t texture;
    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_2D, texture);
    glTexImage2D(GL_TEXTURE_2D, 0, screenChannel, width, height, 0, imageChannel, GL_UNSIGNED_BYTE, &data[0]);
    glGenerateMipmap(GL_TEXTURE_2D);

    // set the texture wrapping/filtering options (on the currently bound texture object)
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAX_ANISOTROPY, 16);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, minFilter);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, filterChannel);

    return Texture(texture, Vector2I(width, height));
}
Texture Texture::get(const std::string &path)
{
    if(!loadedTextures.count(path))
    {
        std::cout << "ERROR :: Texture at \'" << path << "\' could not be found." << std::endl;
        return loadedTextures.at("default");
    }
    return loadedTextures.at(path);
}
std::vector<Texture> Texture::get(const std::string &path, const std::vector<std::string> &subPaths, Type type)
{
    std::vector<Texture> textures;
    for (std::string subPath : subPaths)
    {
        std::string pathName = path + subPath + "." + typeToString(type);
        if(!loadedTextures.count(pathName))
        {
            std::cout << "ERROR :: Texture at \'" << pathName << "\' could not be found." << std::endl;
            return std::vector<Texture>();
        }
        textures.push_back(loadedTextures[pathName]);
    }
    return textures;
}
void Texture::clear()
{
    for (auto &pair : loadedTextures)
    {
        glDeleteTextures(1, &pair.second.texture);
    }
}