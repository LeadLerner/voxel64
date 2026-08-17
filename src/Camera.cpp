#include "Camera.hpp"

Camera::Camera() {
    cameraPos.x = 0.0f;
    cameraPos.y = 0.0f;
    cameraPos.z = 0.0f;
}

Camera::Camera(float x, float y, float z) {
    cameraPos.x = x;
    cameraPos.y = y;
    cameraPos.z = z;
}

void Camera::moveForward(float deltaTime, float speed)
{
    cameraPos += speed * deltaTime * cameraFront;
}

void Camera::moveBackward(float deltaTime, float speed)
{
    cameraPos -= speed * deltaTime * cameraFront;
}

void Camera::moveLeft(float deltaTime, float speed)
{
    cameraPos -= glm::normalize(glm::cross(cameraFront, cameraUp)) * speed * deltaTime;
}

void Camera::moveRight(float deltaTime, float speed)
{
    cameraPos += glm::normalize(glm::cross(cameraFront, cameraUp)) * speed * deltaTime;
}

void Camera::lookYaw(float deltaTime, float lookValue)
{
    cameraDirection.y += lookValue * deltaTime;
    if (cameraDirection.y > 360)
    {
        cameraDirection.y = 0;
    }
    if (cameraDirection.y < 0)
    {
        cameraDirection.y = 360;
    }
}

void Camera::lookPitch(float deltaTime, float lookValue)
{
    cameraDirection.x += lookValue * deltaTime;
    if (cameraDirection.x > 360)
    {
        cameraDirection.x = 0;
    }
    if (cameraDirection.x < 0)
    {
        cameraDirection.x = 360;
    }
}

void Camera::update()
{
    front.x = cos(glm::radians(cameraDirection.y)) * cos(glm::radians(cameraDirection.x));
    front.y = sin(glm::radians(cameraDirection.x));
    front.z = sin(glm::radians(cameraDirection.y)) * cos(glm::radians(cameraDirection.x));

    cameraFront = glm::normalize(front);

    cameraRight = glm::normalize(glm::cross(cameraFront, up));
    cameraUp = glm::normalize(glm::cross(cameraRight, cameraFront));
}

void Camera::moveAndOrient()
{
    gluLookAt(
        cameraPos.x, cameraPos.y, cameraPos.z,
        cameraPos.x + cameraFront.x, cameraPos.y + cameraFront.y, cameraPos.z + cameraFront.z,
        cameraUp.x, cameraUp.y, cameraUp.z);
}