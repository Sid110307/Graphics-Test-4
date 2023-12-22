#include "include/shader.h"

Shader::Shader(const GLchar* vertexPath, const GLchar* fragmentPath)
{
    std::ifstream vertexShaderFile;
    vertexShaderFile.open(vertexPath);
    if (!vertexShaderFile.is_open())
    {
        std::cerr << "Failed to open vertex shader file!" << std::endl;
        return;
    }

    std::stringstream vertexShaderStream;
    vertexShaderStream << vertexShaderFile.rdbuf();
    vertexShaderFile.close();

    std::string vertexShaderCode = vertexShaderStream.str();
    const GLchar* vertexShaderSource = vertexShaderCode.c_str();

    std::ifstream fragmentShaderFile;
    fragmentShaderFile.open(fragmentPath);
    if (!fragmentShaderFile.is_open())
    {
        std::cerr << "Failed to open fragment shader file!" << std::endl;
        return;
    }

    std::stringstream fragmentShaderStream;
    fragmentShaderStream << fragmentShaderFile.rdbuf();
    fragmentShaderFile.close();

    std::string fragmentShaderCode = fragmentShaderStream.str();
    const GLchar* fragmentShaderSource = fragmentShaderCode.c_str();

    GLuint vertexShader, fragmentShader;
    GLint success;
    GLchar infoLog[512];

    vertexShader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertexShader, 1, &vertexShaderSource, nullptr);
    glCompileShader(vertexShader);

    glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);
    if (!success)
    {
        glGetShaderInfoLog(vertexShader, 512, nullptr, infoLog);
        std::cerr << "Failed to compile vertex shader!" << std::endl;
        std::cerr << infoLog << std::endl;

        return;
    }

    fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragmentShader, 1, &fragmentShaderSource, nullptr);
    glCompileShader(fragmentShader);

    glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &success);
    if (!success)
    {
        glGetShaderInfoLog(fragmentShader, 512, nullptr, infoLog);
        std::cerr << "Failed to compile fragment shader!" << std::endl;
        std::cerr << infoLog << std::endl;

        return;
    }

    ID = glCreateProgram();
    glAttachShader(ID, vertexShader);
    glAttachShader(ID, fragmentShader);
    glLinkProgram(ID);

    glGetProgramiv(ID, GL_LINK_STATUS, &success);
    if (!success)
    {
        glGetProgramInfoLog(ID, 512, nullptr, infoLog);
        std::cerr << "Failed to link shader program!" << std::endl;
        std::cerr << infoLog << std::endl;

        return;
    }

    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);
}

void Shader::use() const
{
    glUseProgram(ID);
}

void Shader::setMatrices(glm::mat4 view, glm::mat4 projection) const
{
    glm::mat4 modelMatrix = glm::mat4(1.0f);
    modelMatrix = glm::translate(modelMatrix, glm::vec3(0.0f, 0.0f, 0.0f));
    modelMatrix = glm::rotate(modelMatrix, glm::radians(0.0f), glm::vec3(0.0f, 1.0f, 0.0f));

    setMat4("model", modelMatrix);
    setMat4("view", view);
    setMat4("projection", projection);
}

void Shader::setBool(const std::string &name, GLboolean value) const
{
    glUniform1i(glGetUniformLocation(ID, name.c_str()), static_cast<GLint>(value));
}

void Shader::setInt(const std::string &name, GLint value) const
{
    glUniform1i(glGetUniformLocation(ID, name.c_str()), value);
}

void Shader::setFloat(const std::string &name, GLfloat value) const
{
    glUniform1f(glGetUniformLocation(ID, name.c_str()), value);
}

void Shader::setVec2(const std::string &name, glm::vec2 value) const
{
    glUniform2fv(glGetUniformLocation(ID, name.c_str()), 1, glm::value_ptr(value));
}

void Shader::setVec3(const std::string &name, glm::vec3 value) const
{
    glUniform3fv(glGetUniformLocation(ID, name.c_str()), 1, glm::value_ptr(value));
}

void Shader::setMat4(const std::string &name, glm::mat4 value) const
{
    glUniformMatrix4fv(glGetUniformLocation(ID, name.c_str()), 1, GL_FALSE, glm::value_ptr(value));
}
