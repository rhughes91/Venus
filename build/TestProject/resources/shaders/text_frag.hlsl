#version 330 core

in vec2 TexCoords;
uniform vec4 objColor;

out vec4 FragColor;

void main()
{             
    FragColor = objColor;
}  