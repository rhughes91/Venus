#version 460 core

in vec2 TexCoords;

uniform sampler2D text;
uniform vec4 objColor;

out vec4 FragColor;

void main()
{             
    FragColor = texture(text, TexCoords) * objColor;
}  