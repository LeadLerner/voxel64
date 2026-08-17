#ifndef ENGINE_HPP
#define ENGINE_HPP

#include <libdragon.h>
#include <GL/gl.h>
#include <GL/glu.h>
#include <GL/gl_integration.h>
#include "glm/glm.hpp"

class Engine
{
private:
    float basePlayerSpeed = 10.0f;
    // probably going to make a camera class. This comes from https://learnopengl.com/Getting-started/Camera
    glm::vec3 cameraPos = glm::vec3(0.0f, 0.0f, 0.3f);
    glm::vec3 cameraTarget = glm::vec3(0.0f, 0.0f, 0.0f);
    glm::vec3 cameraDirection = glm::normalize(cameraPos - cameraTarget);

    glm::vec3 up = glm::vec3(0.0f, 1.0f, 0.0f);
    glm::vec3 cameraRight = glm::normalize(glm::cross(up, cameraDirection));
    glm::vec3 cameraFront = glm::vec3(0.0f,0.0f,-1.0f);
    glm::vec3 cameraUp = glm::cross(cameraDirection, cameraRight);
    glm::vec3 front;

    // pitch
    float playerDirX = 0.0f;
    // yaw
    float playerDirY = 0.0f;

    float playerCenterX;
    float playerCenterY;
    float playerCenterZ;

    float deltaTime = 0.0f;

    // i think official games had a deadzone of 7. Not going to check though
    unsigned char joystickDeadzone = 7;

    joypad_buttons_t tappedButton;
    joypad_buttons_t heldButton;

    joypad_inputs_t controllerInputs;

    GLfloat blockVertices[8 * 3] = {
        0.0f, 0.0f, 0.0f,
        0.0f, 1.0f, 0.0f,
        1.0f, 1.0f, 0.0f,
        1.0f, 0.0f, 0.0f,

        0.0f, 0.0f, -1.0f,
        0.0f, 1.0f, -1.0f,
        1.0f, 1.0f, -1.0f,
        1.0f, 0.0f, -1.0f};

    // seperating blocks by face to hide non visible faces. Possibly going to implement greedy meshing. We'll see.

    GLubyte blockBackIndices[6] = {
        7, 5, 6,
        7, 4, 5};

    GLubyte blockFrontIndices[6] = {
        0, 2, 1,
        0, 3, 2};

    GLubyte blockLeftIndices[6] = {
        4, 0, 1,
        4, 1, 5};

    GLubyte blockRightIndices[6] = {
        3, 7, 6,
        3, 6, 2};

    GLubyte blockBottomIndices[6] = {
        0, 4, 7,
        0, 7, 3};

    GLubyte blockTopIndices[6] = {
        1, 2, 6,
        1, 6, 5};

    void update();
    void render();

public:
    Engine();
    void run();
};

#endif