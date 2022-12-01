#version 460 core

#define MAX_POINT 0
#define MAX_SPOT 32

struct Material
{
    sampler2D diffuse;
    sampler2D specular;
    float shininess;

    vec3 ambientStrength;
    vec3 diffuseStrength;
    vec3 specularStrength;
};

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


in vec3 Normal;
in vec3 FragPos;
in vec4 FragPosLightSpace;
out vec4 FragColor;

in vec2 TexCoord;

uniform Material material;
uniform sampler2D shadowMap;

uniform bool advanced;
uniform vec3 lightPos, viewPos;
uniform vec4 objColor;

float ShadowCalculation(vec4 fragPosLightSpace, vec3 normal);
vec4 calcDirLight(DirLight light, vec4 color, vec3 normal, vec3 viewDir, float shadow);
vec4 calcPointLight(PointLight light, vec4 color, vec3 normal, vec3 viewDir, float shadow);
vec4 calcSpotLight(SpotLight light, vec4 color, vec3 normal, vec3 viewDir, float shadow);

void main()
{ 
    vec3 norm = normalize(Normal);
    vec3 viewDir = normalize(viewPos - FragPos);
    vec4 color =  texture(material.diffuse, TexCoord) * objColor;

    // float shadow = ShadowCalculation(FragPosLightSpace, norm);
    float shadow = 0;
    vec4 result = color * 0.0;
    result += calcDirLight(dirLight, color, norm, viewDir, shadow);
    if(advanced)
    {
        for(int i = 0; i < totalPointLights; i++)
            result += calcPointLight(pointLights[i], color, norm, viewDir, shadow);
        for(int i = 0; i < totalSpotLights; i++)
            result += calcSpotLight(spotLights[i], color, norm, viewDir, shadow);
    }

    FragColor = result;
}

float ShadowCalculation(vec4 fragPosLightSpace, vec3 normal)
{
    vec3 projCoords = fragPosLightSpace.xyz / fragPosLightSpace.w;
    projCoords = projCoords * 0.5 + 0.5;

    float closestDepth = texture(shadowMap, projCoords.xy).r; 
    float currentDepth = projCoords.z;

    vec3 lightDir = normalize(lightPos - FragPos);
    float bias = max(0.006 * (1.0 - dot(Normal, lightDir)), 0.005);
    
    float shadow = currentDepth - bias > closestDepth  ? 1.0 : 0.0;
    vec2 texelSize = 1.0 / textureSize(shadowMap, 0);
    for(int x = -1; x <= 1; ++x)
    {
        for(int y = -1; y <= 1; ++y)
        {
            float pcfDepth = texture(shadowMap, projCoords.xy + vec2(x, y) * texelSize).r; 
            shadow += currentDepth - bias > pcfDepth  ? 1.0 : 0.0;        
        }    
    }
    shadow /= 9.0;
    
    if(projCoords.z > 1.0)
        shadow = 0.0;
        
    return shadow;
}

vec4 calcDirLight(DirLight light, vec4 color, vec3 normal, vec3 viewDir, float shadow)
{
    vec3 lightDir = normalize(light.direction);
    // diffuse shading
    float diff = max(dot(normal, lightDir), 0.0);
    // specular shading
    vec3 reflectDir = reflect(-lightDir, normal);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);
    // combine results
    vec3 ambient = material.ambientStrength * color.rgb;
    vec3 diffuse = material.diffuseStrength * diff * color.rgb;
    vec3 specular = material.specularStrength * spec * color.rgb;
    return vec4((ambient + (1.0 - shadow) * (ambient + diffuse + specular)) * light.strength, color.a);
}

vec4 calcPointLight(PointLight light, vec4 color, vec3 normal, vec3 viewDir, float shadow)
{
    vec3 lightDir = normalize(light.position - FragPos);
    vec3 halfwayDir = normalize(lightDir + viewDir);

    float diff = max(dot(normal, lightDir), 0.0);

    float spec = pow(max(dot(normal, halfwayDir), 0.0), material.shininess);

    float distance = length(light.position - FragPos);
    float attenuation = 1.0f / (light.constant + light.linear*distance + light.quadratic*(distance*distance));

    vec4 diffuse = vec4(material.diffuseStrength, 1.0f) * diff * attenuation * light.color;
    vec4 specular = vec4(material.specularStrength, 1.0f) * spec * attenuation * light.color;
    
    vec4 result = ((1.0 - shadow) * (diffuse + specular)) * (color);
    return vec4(result.x * light.strength, result.y * light.strength, result.z * light.strength, result.a);
}

vec4 calcSpotLight(SpotLight light, vec4 color, vec3 normal, vec3 viewDir, float shadow)
{
    vec3 lightDir = normalize(light.position - FragPos);
    
    float diff = max(dot(normal, lightDir), 0.0);
    
    vec3 reflectDir = reflect(-lightDir, normal);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);
    
    float distance = length(light.position - FragPos);
    float attenuation = 1.0 / (light.constant + light.linear * distance + light.quadratic * (distance * distance));    
    
    float theta = dot(lightDir, normalize(-light.direction)); 
    float epsilon = light.cutOff - light.outerCutOff;
    float intensity = clamp((theta - light.outerCutOff) / epsilon, 0.0, 1.0);
    // combine results
    vec3 diffuse = material.diffuseStrength * diff * attenuation * intensity * vec3(color);
    vec3 specular = material.specularStrength * spec * attenuation * intensity * vec3(color);
    return vec4((1.0 - shadow) * (diffuse + specular) * light.strength, color.a);
}