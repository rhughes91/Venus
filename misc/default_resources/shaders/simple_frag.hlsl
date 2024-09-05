#version 330 core

struct Material
{
    sampler2D texture;
};

in vec3 FragPos;
out vec4 FragColor;

in vec2 TexCoord;

uniform Material material;
uniform vec4 objColor;
uniform bool flip;

void main()
{ 
    FragColor = texture(material.texture, vec2((float(flip) * -2 + 1) * TexCoord.x, TexCoord.y)) * objColor;
}