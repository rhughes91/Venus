#ifndef SHADER_H
#define SHADER_H

#include <string>
#include <stdint.h>
#include "vector.h"
#include "color.h"

// Shader (struct): wrapper for graphical shader data :: allows for .hlsl files to be updated and used
struct Shader
{
    uint32_t ID;

    Shader() {}
    Shader(std::string vertexPath, std::string fragmentPath);
        
    void initialize(int32_t numPoints, int32_t numSpots);
    void refresh(int32_t numPoints, int32_t numSpots);
    void use() const;
    void remove() const;
    
    void setBool(const std::string &name, bool value) const;  
    void setInt(const std::string &name, int32_t value) const;   
    void setFloat(const std::string &name, float value) const;
    void setVec2(const std::string &name, const Vector2 &vec2) const;
    void setVec3(const std::string &name, const Vector3 &vec3) const;
    void setVec3(const std::string &name, const Color &vec3) const;
    void setVec4(const std::string &name, const Color &vec4) const;
    void setMat4(const std::string &name, const float *, bool) const;

    private:
        std::string m_vertexPath, m_fragmentPath;
        uint32_t compileShader(const std::string &contents, uint32_t type) const;
};

// Material (struct): holds the information necessary to render an entity to a complex Shader
struct Material
{
    Shader shader;

    Vector3 ambientStrength, diffuseStrength, specularStrength;
    Color specular;
    float shininess;

    // determines whether the object responds to point and spot lights
    bool useAdvancedLighting = true;

    Material(const Shader &shader__ = Shader(), const Vector3 &ambientStrength__ = vec3::zero, const Vector3 &diffuseStrength__ = vec3::zero, const Vector3 &specularStrength__ = vec3::zero, const Color &specular__ = color::WHITE, float shininess__ = 0, bool useAdvancedLighting__ = true)  :
    shader(shader__), ambientStrength(ambientStrength__), diffuseStrength(diffuseStrength__), specularStrength(specularStrength__), specular(specular__), shininess(shininess__), useAdvancedLighting(useAdvancedLighting__) {}
};

// shader (namespace): global methods for loading and accessing shader data from .hlsl files
namespace shader
{
    Shader &set(const std::string& path, const Shader& shader);
    Shader &get(const std::string& path);

    void remove();
};

#endif