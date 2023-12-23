#version 330 core

layout (location = 0) in vec3 lightPos;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

out vec3 FragPos;
out vec3 Normal;

void main()
{
    FragPos = vec3(model * vec4(lightPos, 1.0));
    Normal = mat3(transpose(inverse(model))) * lightPos;

    gl_Position = projection * view * vec4(FragPos, 1.0);
}
