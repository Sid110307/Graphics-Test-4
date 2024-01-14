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
uniform float spotLightAngle;
uniform float shininess = 32.0;

uniform bool enableAmbientLight;
uniform bool enableDiffuseLight;
uniform bool enableSpecularLight;
uniform bool hasTexture;

uniform float ambientStrength = 0.1;
uniform float specularStrength = 0.5;
uniform float linearIntensity = 0.09;
uniform float quadraticIntensity = 0.032;

vec3 calculateAmbientLight()
{
    return enableAmbientLight ? ambientStrength * lightColor : vec3(0.0);
}

vec3 calculateDiffuseLight(vec3 lightPosition)
{
    return enableDiffuseLight ? max(dot(Normal, normalize(lightPosition - FragmentPos)), 0.0) * lightColor : vec3(0.0);
}

vec3 calculateSpecularLight(vec3 lightPosition)
{
    vec3 viewDir = normalize(viewPos - FragmentPos);
    vec3 lightDir = normalize(lightPosition - FragmentPos);
    vec3 reflectDir = reflect(-lightDir, Normal);

    return enableSpecularLight ? specularStrength * pow(max(dot(viewDir, reflectDir), 0.0), shininess) * lightColor : vec3(0.0);
}

void pointLight()
{
    vec3 ambient = calculateAmbientLight();
    vec3 diffuse = calculateDiffuseLight(lightPos);
    vec3 specular = calculateSpecularLight(lightPos);

    float distance = length(lightPos - FragmentPos);
    float intensity = 1.0 / (1.0 + linearIntensity * distance + quadraticIntensity * (distance * distance));

    vec3 diffuseTex = hasTexture ? texture(texture_diffuse1, TexCoords).rgb * (ambient + diffuse) : objectColor * (ambient + diffuse);
    vec3 specularTex = hasTexture ? texture(texture_specular1, TexCoords).rgb * specular : specularStrength * specular;

    FragColor = vec4((diffuseTex + specularTex) * intensity, 1.0);
}

void directionalLight()
{
    vec3 ambient = calculateAmbientLight();
    vec3 diffuse = calculateDiffuseLight(vec3(0.0));
    vec3 specular = calculateSpecularLight(vec3(0.0));

    vec3 diffuseTex = hasTexture ? texture(texture_diffuse1, TexCoords).rgb * (ambient + diffuse) : objectColor * (ambient + diffuse);
    vec3 specularTex = hasTexture ? texture(texture_specular1, TexCoords).rgb * specular : specularStrength * specular;

    FragColor = vec4((diffuseTex + specularTex), 1.0);
}

void spotLight()
{
    float outerCone = cos(radians(spotLightAngle)), innerCone = cos(radians(spotLightAngle - 7.5));

    vec3 ambient = calculateAmbientLight();
    vec3 diffuse = calculateDiffuseLight(lightPos);
    vec3 specular = calculateSpecularLight(lightPos);

    vec3 lightDir = normalize(lightPos - FragmentPos);
    float theta = dot(lightDir, normalize(-lightDirection));

    float intensity = clamp((theta - outerCone) / (innerCone - outerCone), 0.0, 1.0);

    vec3 diffuseTex = hasTexture ? texture(texture_diffuse1, TexCoords).rgb * (ambient + diffuse) : objectColor * (ambient + diffuse);
    vec3 specularTex = hasTexture ? texture(texture_specular1, TexCoords).rgb * specular : specularStrength * specular;

    FragColor = vec4((diffuseTex + specularTex) * intensity, 1.0);
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
