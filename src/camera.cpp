#include "include/camera.h"

Camera::Camera(glm::vec3 position, glm::vec3 up, GLfloat yaw, GLfloat pitch)
        : position(position), orientation(glm::vec3(0.0f, 0.0f, -1.0f)), up(up), right(glm::vec3(0.0f)), pitch(pitch),
          yaw(yaw), movementSpeed(SPEED), mouseSensitivity(SENSITIVITY), fov(FOV)
{
    updateCameraVectors();
}

glm::mat4 Camera::getViewMatrix() const
{
    return glm::lookAt(position, position + orientation, up);
}

glm::mat4 Camera::getProjectionMatrix(GLfloat aspectRatio, GLfloat fov, GLfloat near, GLfloat far)
{
    return glm::perspective(glm::radians(fov), aspectRatio, near, far);
}

void Camera::processKeyboard(CameraMovement direction, GLdouble deltaTime)
{
    auto velocity = static_cast<GLfloat>(movementSpeed * deltaTime);

    if (direction == FORWARD) position += orientation * velocity;
    if (direction == BACKWARD) position -= orientation * velocity;
    if (direction == LEFT) position -= right * velocity;
    if (direction == RIGHT) position += right * velocity;
}

void Camera::processMouseMovement(GLfloat xOffset, GLfloat yOffset)
{
    xOffset *= mouseSensitivity;
    yOffset *= mouseSensitivity;

    yaw += xOffset;
    pitch += yOffset;

    if (pitch > 89.0f) pitch = 89.0f;
    if (pitch < -89.0f) pitch = -89.0f;

    updateCameraVectors();
}

void Camera::processMouseScroll(GLfloat yOffset)
{
    if (fov >= 1.0f && fov <= 180.0f) fov -= yOffset;
    if (fov <= 1.0f) fov = 1.0f;
    if (fov >= 180.0f) fov = 180.0f;
}

void Camera::resetProperties()
{
    position = glm::vec3(0.0f, 0.0f, 3.0f);
    orientation = glm::vec3(0.0f, 0.0f, -1.0f);
    up = glm::vec3(0.0f, 1.0f, 0.0f);
    right = glm::vec3(0.0f);

    pitch = 0.0f;
    yaw = 0.0f;

    movementSpeed = SPEED;
    mouseSensitivity = SENSITIVITY;
    fov = FOV;
}

void Camera::updateCameraVectors()
{
    glm::vec3 newOrientation;
    newOrientation.x = std::cos(glm::radians(yaw)) * std::cos(glm::radians(pitch));
    newOrientation.y = std::sin(glm::radians(pitch));
    newOrientation.z = std::sin(glm::radians(yaw)) * std::cos(glm::radians(pitch));

    orientation = glm::normalize(newOrientation);
    right = glm::normalize(glm::cross(orientation, up));
}

glm::vec3 Camera::getPosition() const { return position; }
glm::vec3 Camera::getOrientation() const { return glm::normalize(orientation); }
glm::vec3 Camera::getUp() const { return up; }
glm::vec3 Camera::getRight() const { return glm::normalize(glm::cross(orientation, up)); }
