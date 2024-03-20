#version 330 core

layout (location = 0) in vec2 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTexCoord;

uniform float gamma;

out vec2 TexCoords;
out float inverseGamma;

void main()
{
    gl_Position = vec4(aPos.x * 2, aPos.y * 2, 0.0, 1.0); 
    TexCoords = aTexCoord;
    inverseGamma = 1.0/gamma;
}  