#version 330 core

#define MAX_POINT 32
#define MAX_SPOT 32

struct Material
{
    sampler2D diffuse;
    sampler2D specular;

    float shininess;
    float ambientStrength;
    float diffuseStrength;
    float specularStrength;
};
uniform Material material;

struct DirLight
{
    vec3 direction;
    vec4 color;

    float strength;
};
uniform DirLight dirLight;

struct PointLight
{
    vec3 position;
    vec4 color;

    float strength;
    float constant;
    float linear;
    float quadratic;
};
uniform PointLight pointLights[MAX_POINT+1];
uniform int totalPointLights;

struct SpotLight
{
    vec3 position;
    vec3 direction;
    vec4 color;

    float strength;
    float constant;
    float linear;
    float quadratic;
    float cutOff, outerCutOff;
};
uniform SpotLight spotLights[MAX_SPOT+1];
uniform int totalSpotLights;

uniform vec3 viewPos;
uniform vec4 objColor;

in vec3 Normal;
in vec3 FragPos;
in vec2 TexCoord;

out vec4 FragColor;

vec4 calcDirLight(vec4 color, vec3 normal, vec3 viewDir);
vec4 calcPointLight(PointLight light, vec4 color, vec3 normal, vec3 viewDir);
vec4 calcSpotLight(SpotLight light, vec4 color, vec3 normal, vec3 viewDir);

void main()
{ 
    vec3 norm = normalize(Normal);
    vec3 viewDir = normalize(viewPos - FragPos);
    vec4 color =  texture(material.diffuse, TexCoord) * objColor;

    vec4 result = vec4(0, 0, 0, objColor.a);
    result += calcDirLight(color, norm, viewDir);
    for(int i = 0; i < totalPointLights; i++)
        result += calcPointLight(pointLights[i], color, norm, viewDir);
    for(int i = 0; i < totalSpotLights; i++)
        result += calcSpotLight(spotLights[i], color, norm, viewDir);
    FragColor = result;
}

vec4 calcDirLight(vec4 color, vec3 normal, vec3 viewDir)
{
    vec4 finalColor = color * max(0, dot(dirLight.direction, -normal)) * material.diffuseStrength;
    finalColor += color * pow(max(0, dot(normalize(viewDir + dirLight.direction), -normal)), material.shininess) * material.specularStrength;
    
    return (finalColor * dirLight.color + color * material.ambientStrength) * dirLight.strength;
}

vec4 calcPointLight(PointLight light, vec4 color, vec3 normal, vec3 viewDir)
{
    vec3 lightDir = normalize(light.position - FragPos);
    vec3 halfwayDir = normalize(lightDir + viewDir);

    float diff = max(dot(normal, lightDir), 0.0);
    float spec = pow(max(dot(normal, halfwayDir), 0.0), material.shininess);

    float distance = length(light.position - FragPos);
    float attenuation = 1.0f / (light.constant + light.linear*distance + light.quadratic*(distance*distance));

    vec3 diffuse = material.diffuseStrength * diff * attenuation * vec3(color) * vec3(light.color);
    vec3 specular = material.specularStrength * spec * attenuation * vec3(color) * vec3(light.color);

    return vec4((diffuse + specular) * light.strength, color.a);
}

vec4 calcSpotLight(SpotLight light, vec4 color, vec3 normal, vec3 viewDir)
{
    vec3 lightDir = normalize(light.position - FragPos);
    vec3 reflectDir = reflect(-lightDir, normal);
    
    float diff = max(dot(normal, lightDir), 0.0);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);
    
    float distance = length(light.position - FragPos);
    float attenuation = 1.0 / (light.constant + light.linear * distance + light.quadratic * (distance * distance));    
    
    float theta = dot(lightDir, normalize(-light.direction)); 
    float epsilon = light.cutOff - light.outerCutOff;
    float intensity = clamp((theta - light.outerCutOff) / epsilon, 0.0, 1.0);
    
    vec3 diffuse = material.diffuseStrength * diff * attenuation * intensity * vec3(color) * vec3(light.color);
    vec3 specular = material.specularStrength * spec * attenuation * intensity * vec3(color) * vec3(light.color);

    return vec4((diffuse + specular) * light.strength, color.a);
}