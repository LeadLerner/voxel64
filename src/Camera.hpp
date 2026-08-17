#ifndef CAMERA_H
#define CAMERA_H

#include "glm/glm/glm.hpp"
#include "GL/glu.h"

class Camera
{
private:
    glm::vec3 cameraPos = glm::vec3(0.0f, 0.0f, 0.3f);
    glm::vec3 cameraTarget = glm::vec3(0.0f, 0.0f, 0.0f);
    glm::vec3 cameraDirection = glm::normalize(cameraPos - cameraTarget);

    glm::vec3 up = glm::vec3(0.0f, 1.0f, 0.0f);
    glm::vec3 cameraRight = glm::normalize(glm::cross(up, cameraDirection));
    glm::vec3 cameraFront = glm::vec3(0.0f, 0.0f, -1.0f);
    glm::vec3 cameraUp = glm::cross(cameraDirection, cameraRight);
    glm::vec3 front;

public:
    Camera();
    Camera(float x, float y, float z);

    void moveForward(float deltaTime, float speed);
    void moveBackward(float deltaTime, float speed);
    void moveLeft(float deltaTime, float speed);
    void moveRight(float deltaTime, float speed);

    void lookYaw(float deltaTime, float lookValue);
    void lookPitch(float deltaTime, float lookValue);

    void update();

    void moveAndOrient();
};

#endif