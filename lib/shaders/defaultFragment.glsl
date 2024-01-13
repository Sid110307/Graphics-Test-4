#version 330 core

out vec4 FragColor;

in vec3 Normal;
in vec3 FragmentPos;
in vec2 TexCoords;

uniform vec3 lightPos;
uniform vec3 viewPos;
uniform sampler2D texture_diffuse1;

uniform float ambientStrength = 0.1;
uniform float diffuseStrength = 0.5;
uniform float specularStrength = 1.0;

void main()
{
    vec3 ambient = ambientStrength * vec3(texture(texture_diffuse1, TexCoords));

    vec3 norm = normalize(Normal);
    vec3 lightDir = normalize(lightPos - FragmentPos);
    vec3 diffuse = diffuseStrength * max(dot(norm, lightDir), 0.0) * vec3(texture(texture_diffuse1, TexCoords));

    vec3 viewDir = normalize(viewPos - FragmentPos);
    vec3 reflectDir = reflect(-lightDir, norm);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), 32);
    vec3 specular = specularStrength * spec * vec3(texture(texture_diffuse1, TexCoords));

    vec3 result = (ambient + diffuse + specular) * vec3(texture(texture_diffuse1, TexCoords));
    FragColor = vec4(result, 1.0);
}
