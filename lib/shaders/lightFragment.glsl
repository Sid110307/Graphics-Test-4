#version 330 core

out vec4 FragColor;

in vec3 normal;
in vec3 FragPos;

uniform vec3 lightColor;
uniform vec3 lightDirection;
uniform vec3 lightPosition;
uniform vec3 viewPos;
uniform vec3 objectColor;

uniform float cutOff;
uniform float outerCutOff;

uniform vec3 ambient = vec3(0.1, 0.1, 0.1);

void directionalLight()
{
    vec3 lightDir = normalize(-lightDirection);
    float diff = max(dot(normal, lightDir), 0.0);
    vec3 diffuse = diff * lightColor;

    vec3 viewDir = normalize(-viewPos);
    vec3 reflectDir = reflect(-lightDir, normal);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), 32);
    vec3 specular = spec * lightColor;

    vec3 result = (ambient + diffuse + specular) * objectColor;
    FragColor = vec4(result, 1.0);
}

void pointLight()
{
    vec3 lightDir = normalize(lightPosition - FragPos);
    float diff = max(dot(normal, lightDir), 0.0);
    vec3 diffuse = diff * lightColor;

    vec3 viewDir = normalize(-viewPos);
    vec3 reflectDir = reflect(-lightDir, normal);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), 32);
    vec3 specular = spec * lightColor;

    float distance = length(lightPosition - FragPos);
    float attenuation = 1.0 / (1.0 + 0.09 * distance + 0.032 * (distance * distance));
    vec3 result = (ambient + diffuse + specular) * objectColor * attenuation;
    FragColor = vec4(result, 1.0);
}

void spotLight()
{
    vec3 lightDir = normalize(lightPosition - FragPos);
    float diff = max(dot(normal, lightDir), 0.0);
    vec3 diffuse = diff * lightColor;

    vec3 viewDir = normalize(-viewPos);
    vec3 reflectDir = reflect(-lightDir, normal);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), 32);
    vec3 specular = spec * lightColor;

    float distance = length(lightPosition - FragPos);
    float attenuation = 1.0 / (1.0 + 0.09 * distance + 0.032 * (distance * distance));

    float theta = dot(lightDir, normalize(-lightDirection));
    float intensity = clamp((theta - outerCutOff) / (cutOff - outerCutOff), 0.0, 1.0);
    diffuse *= intensity;
    specular *= intensity;

    vec3 result = (ambient + diffuse + specular) * objectColor * attenuation;
    FragColor = vec4(result, 1.0);
}

void main()
{
    directionalLight();
    // pointLight();
    // spotLight();
}
