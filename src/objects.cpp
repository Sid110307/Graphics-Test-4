#include "include/objects.h"

Object::Object(Shader &shader) : shader(shader) {}

Object::~Object()
{
    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
}

void Object::updateModel()
{
    model = glm::mat4(1.0f);
    model = glm::translate(model, position);
    model = glm::rotate(model, glm::radians(rotation.x), glm::vec3(1.0f, 0.0f, 0.0f));
    model = glm::rotate(model, glm::radians(rotation.y), glm::vec3(0.0f, 1.0f, 0.0f));
    model = glm::rotate(model, glm::radians(rotation.z), glm::vec3(0.0f, 0.0f, 1.0f));
    model = glm::scale(model, scale);
}

Cube::Cube(Shader &shader) : Object(shader)
{
    vertices = {
            glm::vec3(-0.5f, -0.5f, 0.5f),
            glm::vec3(0.5f, -0.5f, 0.5f),
            glm::vec3(0.5f, 0.5f, 0.5f),
            glm::vec3(-0.5f, 0.5f, 0.5f),

            glm::vec3(-0.5f, -0.5f, -0.5f),
            glm::vec3(0.5f, -0.5f, -0.5f),
            glm::vec3(0.5f, 0.5f, -0.5f),
            glm::vec3(-0.5f, 0.5f, -0.5f),
    };
    indices = {
            0, 1, 2, 2, 3, 0,
            1, 5, 6, 6, 2, 1,
            7, 6, 5, 5, 4, 7,
            4, 0, 3, 3, 7, 4,
            4, 5, 1, 1, 0, 4,
            3, 2, 6, 6, 7, 3
    };

    updateModel();

    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);

    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, static_cast<long>(vertices.size() * sizeof(glm::vec3)), vertices.data(),
                 GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, static_cast<long>(indices.size() * sizeof(GLuint)), indices.data(),
                 GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), nullptr);
    glEnableVertexAttribArray(0);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
}

void Cube::draw()
{
    shader.setMat4("model", model);

    glBindVertexArray(VAO);
    glDrawElements(GL_TRIANGLES, static_cast<GLint>(indices.size()), GL_UNSIGNED_INT, nullptr);
    glBindVertexArray(0);
}

Sphere::Sphere(Shader &shader) : Object(shader)
{
    const GLint X_SEGMENTS = 64, Y_SEGMENTS = 64;

    for (GLint y = 0; y <= Y_SEGMENTS; ++y)
        for (GLint x = 0; x <= X_SEGMENTS; ++x)
        {
            GLfloat xSegment = static_cast<GLfloat>(x) / static_cast<GLfloat>(X_SEGMENTS);
            GLfloat ySegment = static_cast<GLfloat>(y) / static_cast<GLfloat>(Y_SEGMENTS);

            GLdouble xPos = std::cos(xSegment * 2.0f * M_PI) * std::sin(ySegment * M_PI);
            GLdouble yPos = std::cos(ySegment * M_PI);
            GLdouble zPos = std::sin(xSegment * 2.0f * M_PI) * std::sin(ySegment * M_PI);

            vertices.emplace_back(static_cast<GLfloat>(xPos), static_cast<GLfloat>(yPos), static_cast<GLfloat>(zPos));
        }

    bool oddRow = false;
    for (GLint y = 0; y < Y_SEGMENTS; ++y)
    {
        if (!oddRow)
            for (GLint x = 0; x <= X_SEGMENTS; ++x)
            {
                indices.emplace_back(y * (X_SEGMENTS + 1) + x);
                indices.emplace_back((y + 1) * (X_SEGMENTS + 1) + x);
            }
        else
            for (GLint x = X_SEGMENTS; x >= 0; --x)
            {
                indices.emplace_back((y + 1) * (X_SEGMENTS + 1) + x);
                indices.emplace_back(y * (X_SEGMENTS + 1) + x);
            }

        oddRow = !oddRow;
    }

    updateModel();

    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);

    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, static_cast<long>(vertices.size() * sizeof(glm::vec3)), vertices.data(),
                 GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, static_cast<long>(indices.size() * sizeof(GLuint)), indices.data(),
                 GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), nullptr);
    glEnableVertexAttribArray(0);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
}

void Sphere::draw()
{
    shader.setMat4("model", model);

    glBindVertexArray(VAO);
    glDrawElements(GL_TRIANGLE_STRIP, static_cast<GLint>(indices.size()), GL_UNSIGNED_INT, nullptr);
    glBindVertexArray(0);
}

Cylinder::Cylinder(Shader &shader) : Object(shader)
{
    const GLint X_SEGMENTS = 64, Y_SEGMENTS = 64;

    for (GLint y = 0; y <= Y_SEGMENTS; ++y)
        for (GLint x = 0; x <= X_SEGMENTS; ++x)
        {
            GLfloat xSegment = static_cast<GLfloat>(x) / static_cast<GLfloat>(X_SEGMENTS);
            GLfloat ySegment = static_cast<GLfloat>(y) / static_cast<GLfloat>(Y_SEGMENTS);

            GLdouble xPos = std::cos(xSegment * 2.0f * M_PI);
            GLdouble yPos = ySegment * 2.0f - 1.0f;
            GLdouble zPos = std::sin(xSegment * 2.0f * M_PI);

            vertices.emplace_back(static_cast<GLfloat>(xPos), static_cast<GLfloat>(yPos), static_cast<GLfloat>(zPos));
        }

    bool oddRow = false;
    for (GLint y = 0; y < Y_SEGMENTS; ++y)
    {
        if (!oddRow)
            for (GLint x = 0; x <= X_SEGMENTS; ++x)
            {
                indices.emplace_back(y * (X_SEGMENTS + 1) + x);
                indices.emplace_back((y + 1) * (X_SEGMENTS + 1) + x);
            }
        else
            for (GLint x = X_SEGMENTS; x >= 0; --x)
            {
                indices.emplace_back((y + 1) * (X_SEGMENTS + 1) + x);
                indices.emplace_back(y * (X_SEGMENTS + 1) + x);
            }

        oddRow = !oddRow;
    }

    updateModel();

    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);

    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, static_cast<long>(vertices.size() * sizeof(glm::vec3)), vertices.data(),
                 GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, static_cast<long>(indices.size() * sizeof(GLuint)), indices.data(),
                 GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), nullptr);
    glEnableVertexAttribArray(0);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
}

void Cylinder::draw()
{
    shader.setMat4("model", model);

    glBindVertexArray(VAO);
    glDrawElements(GL_TRIANGLE_STRIP, static_cast<GLint>(indices.size()), GL_UNSIGNED_INT, nullptr);
    glBindVertexArray(0);
}

Cone::Cone(Shader &shader) : Object(shader)
{
    const GLint X_SEGMENTS = 64, Y_SEGMENTS = 64;

    for (GLint y = 0; y <= Y_SEGMENTS; ++y)
        for (GLint x = 0; x <= X_SEGMENTS; ++x)
        {
            GLfloat xSegment = static_cast<GLfloat>(x) / static_cast<GLfloat>(X_SEGMENTS);
            GLfloat ySegment = static_cast<GLfloat>(y) / static_cast<GLfloat>(Y_SEGMENTS);

            GLdouble xPos = std::cos(xSegment * 2.0f * M_PI) * (1.0f - ySegment);
            GLdouble yPos = ySegment * 2.0f - 1.0f;
            GLdouble zPos = std::sin(xSegment * 2.0f * M_PI) * (1.0f - ySegment);

            vertices.emplace_back(static_cast<GLfloat>(xPos), static_cast<GLfloat>(yPos), static_cast<GLfloat>(zPos));
        }

    bool oddRow = false;
    for (GLint y = 0; y < Y_SEGMENTS; ++y)
    {
        if (!oddRow)
            for (GLint x = 0; x <= X_SEGMENTS; ++x)
            {
                indices.emplace_back(y * (X_SEGMENTS + 1) + x);
                indices.emplace_back((y + 1) * (X_SEGMENTS + 1) + x);
            }
        else
            for (GLint x = X_SEGMENTS; x >= 0; --x)
            {
                indices.emplace_back((y + 1) * (X_SEGMENTS + 1) + x);
                indices.emplace_back(y * (X_SEGMENTS + 1) + x);
            }

        oddRow = !oddRow;
    }

    updateModel();

    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);

    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, static_cast<long>(vertices.size() * sizeof(glm::vec3)), vertices.data(),
                 GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, static_cast<long>(indices.size() * sizeof(GLuint)), indices.data(),
                 GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), nullptr);
    glEnableVertexAttribArray(0);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
}

void Cone::draw()
{
    shader.setMat4("model", model);

    glBindVertexArray(VAO);
    glDrawElements(GL_TRIANGLE_STRIP, static_cast<GLint>(indices.size()), GL_UNSIGNED_INT, nullptr);
    glBindVertexArray(0);
}

Torus::Torus(Shader &shader) : Object(shader)
{
    const GLint X_SEGMENTS = 64, Y_SEGMENTS = 64;

    for (GLint y = 0; y <= Y_SEGMENTS; ++y)
        for (GLint x = 0; x <= X_SEGMENTS; ++x)
        {
            GLfloat xSegment = static_cast<GLfloat>(x) / static_cast<GLfloat>(X_SEGMENTS);
            GLfloat ySegment = static_cast<GLfloat>(y) / static_cast<GLfloat>(Y_SEGMENTS);

            GLdouble xPos = std::cos(xSegment * 2.0f * M_PI);
            GLdouble yPos = std::cos(ySegment * 2.0f * M_PI);
            GLdouble zPos = std::sin(xSegment * 2.0f * M_PI);

            vertices.emplace_back(static_cast<GLfloat>(xPos), static_cast<GLfloat>(yPos), static_cast<GLfloat>(zPos));
        }

    bool oddRow = false;
    for (GLint y = 0; y < Y_SEGMENTS; ++y)
    {
        if (!oddRow)
            for (GLint x = 0; x <= X_SEGMENTS; ++x)
            {
                indices.emplace_back(y * (X_SEGMENTS + 1) + x);
                indices.emplace_back((y + 1) * (X_SEGMENTS + 1) + x);
            }
        else
            for (GLint x = X_SEGMENTS; x >= 0; --x)
            {
                indices.emplace_back((y + 1) * (X_SEGMENTS + 1) + x);
                indices.emplace_back(y * (X_SEGMENTS + 1) + x);
            }

        oddRow = !oddRow;
    }

    updateModel();

    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);

    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, static_cast<long>(vertices.size() * sizeof(glm::vec3)), vertices.data(),
                 GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, static_cast<long>(indices.size() * sizeof(GLuint)), indices.data(),
                 GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), nullptr);
    glEnableVertexAttribArray(0);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
}

void Torus::draw()
{
    shader.setMat4("model", model);

    glBindVertexArray(VAO);
    glDrawElements(GL_TRIANGLE_STRIP, static_cast<GLint>(indices.size()), GL_UNSIGNED_INT, nullptr);
    glBindVertexArray(0);
}

Plane::Plane(Shader &shader) : Object(shader)
{
    vertices = {
            glm::vec3(-0.5f, 0.0f, 0.5f),
            glm::vec3(0.5f, 0.0f, 0.5f),
            glm::vec3(0.5f, 0.0f, -0.5f),
            glm::vec3(-0.5f, 0.0f, -0.5f),
    };
    indices = {0, 1, 2, 2, 3, 0};

    updateModel();

    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);

    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, static_cast<long>(vertices.size() * sizeof(glm::vec3)), vertices.data(),
                 GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, static_cast<long>(indices.size() * sizeof(GLuint)), indices.data(),
                 GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), nullptr);
    glEnableVertexAttribArray(0);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
}

void Plane::draw()
{
    shader.setMat4("model", model);

    glBindVertexArray(VAO);
    glDrawElements(GL_TRIANGLES, static_cast<GLint>(indices.size()), GL_UNSIGNED_INT, nullptr);
    glBindVertexArray(0);
}
