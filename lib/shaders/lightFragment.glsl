#version 330 core

out vec4 FragColor;

in vec3 FragPos;
in vec3 Normal;

uniform int lightType;

uniform vec3 objectColor;
uniform vec3 lightColor;

uniform vec3 lightPosition;
uniform vec3 lightDirection;

uniform bool enableAmbientLight;
uniform bool enableDiffuseLight;
uniform bool enableSpecularLight;

uniform float ambientStrength = 0.1;
uniform float specularStrength = 0.5;
uniform float linearAttenuation = 0.09;
uniform float quadraticAttenuation = 0.032;

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

vec3 calculateDiffuseLight(vec3 norm, vec3 lightDir)
{
    return enableDiffuseLight ? max(dot(norm, lightDir), 0.0) * lightColor : vec3(0.0);
}

vec3 calculateSpecularLight(vec3 norm, vec3 viewDir, vec3 lightDir)
{
    return enableSpecularLight ? pow(max(dot(norm, normalize(viewDir + lightDir)), 0.0), 32) * specularStrength * lightColor : vec3(0.0);
}

void directionalLight()
{
    vec3 ambient = calculateAmbientLight();

    vec3 norm = normalize(Normal);
    vec3 lightDir = normalize(lightDirection);

    vec3 diffuse = calculateDiffuseLight(norm, lightDir);
    vec3 specular = calculateSpecularLight(norm, normalize(-FragPos), lightDir);

    vec3 result = (ambient + diffuse + specular) * objectColor;
    FragColor = vec4(result, 1.0);
}

void pointLight()
{
    vec3 ambient = calculateAmbientLight();

    vec3 norm = normalize(Normal);
    vec3 lightDir = normalize(lightPosition - FragPos);

    vec3 diffuse = calculateDiffuseLight(norm, lightDir);
    vec3 specular = calculateSpecularLight(norm, normalize(-FragPos), lightDir);

    float distance = length(lightPosition - FragPos);
    float attenuation = 1.0 / (1.0 + linearAttenuation * distance + quadraticAttenuation * (distance * distance));

    vec3 result = (ambient + diffuse + specular) * objectColor * attenuation;
    FragColor = vec4(result, 1.0);
}

void spotLight()
{
    vec3 ambient = calculateAmbientLight();

    vec3 norm = normalize(Normal);
    vec3 lightDir = normalize(lightPosition - FragPos);

    vec3 diffuse = calculateDiffuseLight(norm, lightDir);
    vec3 specular = calculateSpecularLight(norm, normalize(-FragPos), lightDir);

    float distance = length(lightPosition - FragPos);
    float attenuation = 1.0 / (1.0 + linearAttenuation * distance + quadraticAttenuation * (distance * distance));

    float theta = dot(lightDir, normalize(-lightDirection));
    float epsilon = lightDirection.x > 0.0 ? 0.001 : -0.001;
    float intensity = clamp((theta - lightDirection.x) / (lightDirection.x - epsilon), 0.0, 1.0);

    vec3 result = (ambient + diffuse + specular) * objectColor * attenuation * intensity;
    FragColor = vec4(result, 1.0);
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
            FragColor = vec4(1.0);
            break;
        }
    }
}
