#version 460 core

#define MAX_POINT 0
#define MAX_SPOT 32

struct Material
{
    sampler2D texture;
};


in vec3 Normal;
in vec3 FragPos;
out vec4 FragColor;

in vec2 TexCoord;

uniform Material material;
uniform vec4 objColor;

void main()
{ 
    FragColor = texture(material.texture, TexCoord) * objColor;
}