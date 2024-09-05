#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 2) in vec2 aTexCoord;

uniform mat4 lightSpaceMatrix;
uniform mat4 model;
uniform vec3 scale;

out vec2 TexCoord;

void main()
{
    gl_Position = lightSpaceMatrix * model * vec4(aPos * scale, 1.0);
    TexCoord = aTexCoord;
}  