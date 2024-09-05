#version 330 core

uniform sampler2D text;
uniform bool clear;

in vec2 TexCoord;
out vec4 FragColor;

void main()
{          
    if(vec4(texture(text, TexCoord)).a < 0.1 || clear)
    {
        discard;
    }
}  