#include "glad/glad.h"
#include <string>

#include "file_util.h"
#include "shader.h"

Shader::Shader(std::string vertexPath, std::string fragmentPath)
{
    m_vertexPath = vertexPath;
    m_fragmentPath = fragmentPath;
    initialize(0, 0);
}
void Shader::initialize(int32_t numPoints, int32_t numSpots)
{
    ID = glCreateProgram();
    uint32_t shader;
    if(shader = compileShader
    (
        loadFileToString(("resources/shaders/"+m_vertexPath+".hlsl").c_str()), GL_VERTEX_SHADER
    ))
    {
        glAttachShader(ID, shader);
        glDeleteShader(shader);
    }
    else return;
    if(shader = compileShader
    (
        loadFileToString(("resources/shaders/"+m_fragmentPath+".hlsl").c_str()), GL_FRAGMENT_SHADER
    ))
    {
        glAttachShader(ID, shader);
        glDeleteShader(shader);
    }
    else return;
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
    glUniform1i(transformLoc, value); 
}
void Shader::setFloat(const std::string &name, float value) const
{
    
    uint32_t transformLoc = glGetUniformLocation(ID, name.c_str());
    // if(transformLoc == -1) std::cout << name << std::endl;
    glUniform1f(transformLoc, value); 
} 
void Shader::setVec2(const std::string &name, const Vector2 &vec2) const
{
    uint32_t transformLoc = glGetUniformLocation(ID, name.c_str());
    float vector[] = {vec2.x, vec2.y};
    glUniform2fv(transformLoc, 1, vector);
}
void Shader::setVec3(const std::string &name, const Vector3 &vec3) const
{
    uint32_t transformLoc = glGetUniformLocation(ID, name.c_str());
    float vector[] = {vec3.x, vec3.y, vec3.z};
    glUniform3fv(transformLoc, 1, vector);
}
void Shader::setVec3(const std::string &name, const Color &vec3) const
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
        std::cout << "ERROR::"<<"SHADER"<<"::COMPILATION_FAILED\n" << infoLog << std::endl;
        return 0;
    }
    return shader;
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