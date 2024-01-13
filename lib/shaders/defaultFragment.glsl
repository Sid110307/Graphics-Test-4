#version 330 core

out vec4 FragColor;

in vec3 FragmentPos;
smooth in vec3 Normal;
in vec3 Color;
in vec2 TexCoords;

uniform sampler2D texture_diffuse1;
uniform sampler2D texture_specular1;

uniform vec3 lightColor;
uniform vec3 lightPos;
uniform vec3 lightDirection;

uniform vec3 viewPos;
uniform vec3 objectColor;
uniform int lightType;
uniform float spotLightAngle = 12.5;
uniform float shininess = 32.0;

uniform bool enableAmbientLight;
uniform bool enableDiffuseLight;
uniform bool enableSpecularLight;
uniform bool hasTexture;

uniform float ambientStrength = 0.1;
uniform float specularStrength = 0.5;
uniform float linearIntensity = 0.09;
uniform float quadraticIntensity = 0.032;

struct Light
{
    vec3 position, direction;
    float constant, linear, quadratic, cutoff;
};

Light createLight(vec3 position, vec3 direction, float constant, float linear, float quadratic, float cutoff)
{
    Light light;
    light.position = position;
    light.direction = direction;
    light.constant = constant;
    light.linear = linear;
    light.quadratic = quadratic;
    light.cutoff = cutoff;

    return light;
}

vec3 calculateAmbientLight()
{
    return enableAmbientLight ? ambientStrength * lightColor : vec3(0.0);
}

vec3 calculateDiffuseLight(Light light)
{
    return enableDiffuseLight ? max(dot(Normal, normalize(light.position - FragmentPos)), 0.0) * lightColor : vec3(0.0);
}

vec3 calculateSpecularLight(Light light)
{
    vec3 viewDir = normalize(viewPos - FragmentPos);
    vec3 lightDir = normalize(light.position - FragmentPos);
    vec3 reflectDir = reflect(-lightDir, Normal);

    return enableSpecularLight ? specularStrength * pow(max(dot(viewDir, reflectDir), 0.0), shininess) * lightColor : vec3(0.0);
}

void pointLight()
{
    Light light = createLight(lightPos, vec3(0.0), 1.0, linearIntensity, quadraticIntensity, 0.0);

    vec3 ambient = calculateAmbientLight();
    vec3 diffuse = calculateDiffuseLight(light);
    vec3 specular = calculateSpecularLight(light);

    float distance = length(light.position - FragmentPos);
    float intensity = 1.0 / (light.constant + light.linear * distance + light.quadratic * (distance * distance));

    vec3 diffuseTex = hasTexture ? texture(texture_diffuse1, TexCoords).rgb * (ambient + diffuse) : objectColor * (ambient + diffuse);
    vec3 specularTex = hasTexture ? texture(texture_specular1, TexCoords).rgb * specularStrength * specular : specularStrength * specular;

    FragColor = vec4((diffuseTex + specularTex) * intensity, 1.0);
}

void directionalLight()
{
    Light light = createLight(vec3(0.0), normalize(lightDirection), 1.0, linearIntensity, quadraticIntensity, 0.0);

    vec3 ambient = calculateAmbientLight();
    vec3 diffuse = calculateDiffuseLight(light);
    vec3 specular = calculateSpecularLight(light);

    vec3 diffuseTex = hasTexture ? texture(texture_diffuse1, TexCoords).rgb * (ambient + diffuse) : objectColor * (ambient + diffuse);
    vec3 specularTex = hasTexture ? texture(texture_specular1, TexCoords).rgb * specularStrength * specular : specularStrength * specular;

    FragColor = vec4((diffuseTex + specularTex), 1.0);
}

void spotLight()
{
    Light light = createLight(lightPos, normalize(lightDirection), 1.0, linearIntensity, quadraticIntensity, cos(radians(spotLightAngle)));

    vec3 ambient = calculateAmbientLight();
    vec3 diffuse = calculateDiffuseLight(light);
    vec3 specular = calculateSpecularLight(light);

    float distance = length(light.position - FragmentPos);
    float intensity = 1.0 / (light.constant + light.linear * distance + light.quadratic * (distance * distance));

    vec3 diffuseTex = hasTexture ? texture(texture_diffuse1, TexCoords).rgb * (ambient + diffuse * intensity) : objectColor * (ambient + diffuse * intensity);
    vec3 specularTex = hasTexture ? texture(texture_specular1, TexCoords).rgb * specularStrength * intensity * specular : specularStrength * intensity * specular;

    FragColor = vec4(diffuseTex + specularTex, 1.0);
}

void main()
{
    switch (lightType)
    {
        case 0:
        {
            pointLight();
            break;
        }
        case 1:
        {
            directionalLight();
            break;
        }
        case 2:
        {
            spotLight();
            break;
        }
        default :
        {
            FragColor = vec4(Color, 1.0);
            break;
        }
    }
}
