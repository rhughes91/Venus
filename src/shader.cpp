#include "shader.h"
#include "setup.h"
#include "graphics.h"
#include "file_util.h"

extern Time g_time;
extern Window g_window;

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