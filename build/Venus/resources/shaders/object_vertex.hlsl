#version 460 core

layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTexCoord;

out vec2 TexCoord;
out vec3 FragPos;
out vec3 Normal;
out vec4 FragPosLightSpace;

uniform mat4 model, view, projection, lightSpaceMatrix;
uniform vec3 scale;

void main()
{
    FragPos = vec3(model * vec4(aPos * scale, 1.0f));
    gl_Position = projection * view * vec4(FragPos, 1.0);
    FragPosLightSpace = lightSpaceMatrix * vec4(FragPos, 1.0);
    TexCoord = aTexCoord;
    Normal = normalize(transpose(inverse(mat3(model))) * aNormal);
}