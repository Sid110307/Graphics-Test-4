#include "include/texture.h"

Texture::Texture(const GLchar* file, const std::string &type)
{
    this->type = type;
    path = file;

    glGenTextures(1, &id);
    glBindTexture(GL_TEXTURE_2D, id);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    int width, height, numChannels;
    unsigned char* data = stbi_load(file, &width, &height, &numChannels, 0);
    if (data)
    {
        GLenum format = 0;
        switch (numChannels)
        {
            case 1:
                format = GL_RED;
                break;
            case 3:
                format = GL_RGB;
                break;
            case 4:
                format = GL_RGBA;
                break;
            default:
                std::cerr << "Invalid number of channels (" << numChannels << ") in texture \"" << file << "\""
                          << std::endl;
                break;
        }

        glTexImage2D(GL_TEXTURE_2D, 0, static_cast<GLint>(format), width, height, 0, format, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);
    } else std::cerr << "Failed to load texture from file \"" << file << "\": " << stbi_failure_reason() << std::endl;

    stbi_image_free(data);
}

Texture::~Texture() { glDeleteTextures(1, &id); }
void Texture::bind(GLuint textureUnit) const
{
    glActiveTexture(textureUnit);
    glBindTexture(GL_TEXTURE_2D, id);
}
