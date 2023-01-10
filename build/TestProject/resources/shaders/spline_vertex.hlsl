#version 330 core

layout (location = 0) in vec3 aPos;

uniform float aspect;
uniform vec2 position;

void main()
{
    gl_Position = vec4(vec3(aPos.x * aspect + position.x, aPos.y + position.y, 0), 1.0);
}