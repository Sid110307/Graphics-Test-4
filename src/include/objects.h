#pragma once

#include <vector>

#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "shader.h"

class Object
{
public:
    explicit Object(Shader &shader);
    ~Object();

    virtual void draw() = 0;

    glm::vec3 position = glm::vec3(0.0f), rotation = glm::vec3(0.0f), scale = glm::vec3(1.0f);
    glm::mat4 model = glm::mat4(1.0f);
    GLuint VAO = 0, VBO = 0, EBO = 0;

    Shader shader;

    std::vector<glm::vec3> vertices;
    std::vector<GLuint> indices;

    void updateModel();
};

class Cube : public Object
{
public:
    explicit Cube(Shader &shader);
    void draw() override;
};

class Sphere : public Object
{
public:
    explicit Sphere(Shader &shader);
    void draw() override;
};

class Cylinder : public Object
{
public:
    explicit Cylinder(Shader &shader);
    void draw() override;
};

class Cone : public Object
{
public:
    explicit Cone(Shader &shader);
    void draw() override;
};

class Torus : public Object
{
public:
    explicit Torus(Shader &shader);
    void draw() override;
};

class Plane : public Object
{
public:
    explicit Plane(Shader &shader);
    void draw() override;
};
