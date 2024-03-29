#version 330 core

layout (location = 0) in vec3 position;
layout (location = 1) in vec3 normal;
layout (location = 2) in vec3 color;
layout (location = 3) in vec2 texCoords;

out vec3 FragmentPos;
smooth out vec3 Normal;
out vec3 Color;
out vec2 TexCoords;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

void main()
{
    FragmentPos = vec3(model * vec4(position, 1.0));
    Normal = mat3(transpose(inverse(model))) * normal;
    Color = color;
    TexCoords = texCoords;

    gl_Position = projection * view * vec4(FragmentPos, 1.0);
}

