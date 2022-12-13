#version 460 core

layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTexCoord;

uniform float aspect;
uniform mat4 model;
uniform vec2 position, scale;

out vec2 TexCoords;

void main()
{
    vec3 pos = vec3(model * vec4(aPos * vec3(scale, 1), 1.0f));
    gl_Position = vec4(vec3(pos.x * aspect + position.x, pos.y + position.y, 0), 1.0);
    TexCoords = aTexCoord;
}  