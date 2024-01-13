#include "include/texture.h"

static std::string getError(GLenum error)
{
    switch (error)
    {
        case GL_INVALID_ENUM:
            return "Invalid enum";
        case GL_INVALID_VALUE:
            return "Invalid value";
        case GL_INVALID_OPERATION:
            return "Invalid operation";
        case GL_INVALID_FRAMEBUFFER_OPERATION:
            return "Invalid framebuffer operation";
        case GL_OUT_OF_MEMORY:
            return "Out of memory";
        default:
            return "Unknown error";
    }
}

Texture::Texture(const GLchar* file, const std::string &type)
{
    this->type = type;
    glGenTextures(1, &id);

    GLint width, height, numComponents;
    auto data = stbi_load(file, &width, &height, &numComponents, 0);

    if (data)
    {
        GLenum format;
        switch (numComponents)
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
                std::cerr << "Unsupported number of components in image: " << numComponents << std::endl;
                stbi_image_free(data);

                return;
        }

        glBindTexture(GL_TEXTURE_2D, id);
        glTexImage2D(GL_TEXTURE_2D, 0, static_cast<GLint>(format), width, height, 0, format, GL_UNSIGNED_BYTE, data);

        if (numComponents == 3) glGenerateMipmap(GL_TEXTURE_2D);
        else
        {
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        }

        stbi_image_free(data);
    } else
    {
        std::cerr << "Failed to load texture from file \"" << file << "\": " << stbi_failure_reason() << std::endl;
        stbi_image_free(data);

        return;
    }

    GLenum error = glGetError();
    if (error != GL_NO_ERROR)
    {
        std::cerr << "Failed to create texture from file \"" << file << "\": " << getError(error) << std::endl;
        glDeleteTextures(1, &id);

        return;
    }
}

Texture::~Texture() { glDeleteTextures(1, &id); }
void Texture::bind(GLuint textureUnit) const
{
    glActiveTexture(textureUnit);
    glBindTexture(GL_TEXTURE_2D, id);
}
