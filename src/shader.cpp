#include "glad/glad.h"
#include <string>
#include <iostream>

#include "file_util.h"
#include "shader.h"
#include "structure.h"

std::unordered_map<std::string, Mesh> g_loadedMeshes;
std::unordered_map<std::string, Shader> g_loadedShaders;

Shader::Shader(std::string vertexPath, std::string fragmentPath)
{
    ID = glCreateProgram();
    uint32_t shader;
    if(shader = compileShader
    (
        file::loadFileToString((source::shader()+vertexPath+".hlsl").c_str()), GL_VERTEX_SHADER
    ))
    {
        glAttachShader(ID, shader);
        glDeleteShader(shader);
    }
    else return;
    if(shader = compileShader
    (
        file::loadFileToString((source::shader()+fragmentPath+".hlsl").c_str()), GL_FRAGMENT_SHADER
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
void Shader::setBool(const std::string &name, bool value) const
{         
    glUniform1i(glGetUniformLocation(ID, name.c_str()), (int)value); 
}
void Shader::setInt(const std::string &name, int32_t value) const
{ 
    uint32_t transformLoc = glGetUniformLocation(ID, name.c_str());
    // if(transformLoc == -1) std::cout << ID << " : " << name << "=" << value << std::endl;
    glUniform1i(transformLoc, value); 
}
void Shader::setFloat(const std::string &name, float value) const
{
    uint32_t transformLoc = glGetUniformLocation(ID, name.c_str());
    // if(transformLoc == -1) std::cout << ID << " : " << name << "=" << value << std::endl;
    glUniform1f(transformLoc, value); 
} 
void Shader::setVec2(const std::string &name, const Vector2 &vec2) const
{
    uint32_t transformLoc = glGetUniformLocation(ID, name.c_str());
    // if(transformLoc == -1) std::cout << ID << " :: " << name << " = " << vec2 << std::endl;
    glUniform2f(transformLoc, vec2.x, vec2.y);
}
void Shader::setVec3(const std::string &name, const Vector3 &vec3) const
{
    uint32_t transformLoc = glGetUniformLocation(ID, name.c_str());
    // if(transformLoc == -1) std::cout << ID << " :: " << name << " = " << vec3 << std::endl;
    glUniform3f(transformLoc, vec3.x, vec3.y, vec3.z);
}
void Shader::setColor3(const std::string &name, const Color &vec3) const
{
    uint32_t transformLoc = glGetUniformLocation(ID, name.c_str());
    glUniform3f(transformLoc, vec3.r, vec3.g, vec3.b);
}
void Shader::setVec4(const std::string &name, const Color &vec4) const
{
    uint32_t transformLoc = glGetUniformLocation(ID, name.c_str());
    glUniform4f(transformLoc, vec4.r, vec4.g, vec4.b, vec4.a);
}
void Shader::setMat4(const std::string &name, const float *transform, bool transpose) const
{
    uint32_t transformLoc = glGetUniformLocation(ID, name.c_str());
    glUniformMatrix4fv(transformLoc, 1, transpose, transform);
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

void shader::load(const std::string& path, const Shader& shader)
{
    g_loadedShaders[path] = shader;
}
Shader &shader::get(const std::string& path)
{
    if(!g_loadedShaders.count(path))
    {
        std::cout << "ERROR :: Shader at \'" << path << "\' could not be found." << std::endl;
        // return mesh::get("square");
    }
    return g_loadedShaders.at(path);
}
void shader::remove()
{
    for (auto &pair : g_loadedShaders)
    {
        pair.second.remove();
    }
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
void Mesh::append(const Transform& parentTransform, const std::vector<MeshModule>& additions)
{
    std::vector<Vertex> newVertices = vertices;
    for(auto module : additions)
    {
        module.transform.position = (mat4x4)parentTransform.rotation.conjugate() * module.transform.position;

        module.transform.position.x /= parentTransform.scale.x == 0 ? 1:parentTransform.scale.x;
        module.transform.position.y /= parentTransform.scale.y == 0 ? 1:parentTransform.scale.y;
        module.transform.position.z /= parentTransform.scale.z == 0 ? 1:parentTransform.scale.z;
        
        module.transform.position = (mat4x4)parentTransform.rotation * module.transform.position;

        for(auto vertex : newVertices)
        {
            vertex.position = (mat4x4)module.transform.rotation * (mat4x4)parentTransform.rotation * vertex.position;
            vertex.position += module.transform.position;
            vertex.position = (mat4x4)parentTransform.rotation.conjugate() * vertex.position;
            vertex.uv = vertex.uv * module.uvScale + module.uvOffset;
            vertices.push_back(vertex);
        }
    }

    std::string addendumPath = "Mesh:"+std::to_string(VAO)+":"+std::to_string(VBO);
    if(!mesh::contains(addendumPath))
    {
        generate();
        addendumPath = "Mesh:"+std::to_string(VAO)+":"+std::to_string(VBO);
    }
    mesh::set(addendumPath, *this);
    refresh();
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

Mesh &mesh::load(const std::string &path)
{
    g_loadedMeshes[path] = file::loadObjFile(path);
    g_loadedMeshes[path].generate();
    return(g_loadedMeshes[path]);
}
Mesh &mesh::load(const std::string& path, const Mesh& mesh)
{
    g_loadedMeshes[path] = mesh;
    g_loadedMeshes[path].generate();
    return(g_loadedMeshes[path]);
}
void mesh::load(const std::string &path, const std::vector<std::string> &subPaths, const std::string &type)
{
    for (std::string subPath : subPaths)
    {
        mesh::load(path + subPath + "." + type);
    }
}
void mesh::set(const std::string& path, const Mesh& mesh)
{
    g_loadedMeshes[path] = mesh;
}
Mesh &mesh::get(const std::string &path)
{
    if(!g_loadedMeshes.count(path))
    {
        std::cout << "ERROR :: Mesh at \'" << path << "\' could not be found." << std::endl;
        return mesh::get("square");
    }
    return g_loadedMeshes.at(path);
}
std::vector<Mesh> mesh::get(const std::string &path, const std::vector<std::string> &subPaths, const std::string &type)
{
    std::vector<Mesh> meshes;
    for (std::string subPath : subPaths)
    {
        meshes.push_back(g_loadedMeshes[path + subPath + "." + type]);
    }
    return meshes;
}
void mesh::remove()
{
    for (auto &pair : g_loadedMeshes)
    {
        pair.second.remove();
    }
}
bool mesh::contains(const std::string& path)
{
    return g_loadedMeshes.count(path);
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
    Mesh result = Mesh(vectors, texture, Vector3(1, 1, 1));

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
