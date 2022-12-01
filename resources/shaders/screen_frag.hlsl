#version 460 core

out vec4 FragColor;
  
in vec2 TexCoords;

uniform sampler2D screenTexture;
uniform float gamma;

void main()
{ 
    FragColor = vec4(vec3(texture(screenTexture, TexCoords)), 1.0);
    FragColor.rgb = pow(FragColor.rgb, vec3(1.0/gamma));
}