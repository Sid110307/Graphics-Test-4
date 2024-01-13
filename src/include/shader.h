#pragma once

#include <iostream>
#include <string>
#include <fstream>
#include <sstream>

#include <GL/glew.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

class Shader
{
public:
    GLuint ID;

    Shader(const GLchar* vertexPath, const GLchar* fragmentPath);
    ~Shader();

    void use() const;
    void setMatrices(glm::mat4 view, glm::mat4 projection) const;

    void setBool(const std::string &name, bool value) const;
    void setInt(const std::string &name, GLint value) const;
    void setFloat(const std::string &name, GLfloat value) const;
    void setVec2(const std::string &name, glm::vec2 value) const;
    void setVec3(const std::string &name, glm::vec3 value) const;
    void setMat4(const std::string &name, glm::mat4 value) const;
};
