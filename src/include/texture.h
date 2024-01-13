#pragma once

#include <stb_image.h>
#include "shader.h"

class Texture
{
public:
    Texture(const GLchar* file, const std::string &type);
    ~Texture();

    void bind(GLuint textureUnit = 0) const;

    GLuint id = 0;
    std::string type, path;
};
