#pragma once

#include <GL/glew.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

constexpr GLfloat SPEED = 5.0f, SENSITIVITY = 0.1f, FOV = 45.0f;

enum CameraMovement
{
    FORWARD,
    BACKWARD,
    LEFT,
    RIGHT
};

class Camera
{
public:
    explicit Camera(glm::vec3 position = glm::vec3(0.0f, 0.0f, 3.0f), glm::vec3 up = glm::vec3(0.0f, 1.0f, 0.0f),
                    GLfloat yaw = 0.0f, GLfloat pitch = 0.0f);

    [[nodiscard]] glm::mat4 getViewMatrix() const;
    static glm::mat4 getProjectionMatrix(GLfloat aspectRatio, GLfloat fov, GLfloat near, GLfloat far);

    void processKeyboard(CameraMovement direction, GLdouble deltaTime);
    void processMouseMovement(GLfloat xOffset, GLfloat yOffset);
    void processMouseScroll(GLfloat yOffset);
    void resetProperties();

    [[nodiscard]] glm::vec3 getPosition() const;
    [[nodiscard]] glm::vec3 getOrientation() const;
    [[nodiscard]] glm::vec3 getUp() const;
    [[nodiscard]] glm::vec3 getRight() const;

    GLfloat pitch, yaw, movementSpeed, mouseSensitivity, fov;

private:
    void updateCameraVectors();
    glm::vec3 position, orientation, up, right;
};
