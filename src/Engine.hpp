#ifndef ENGINE_H
#define ENGINE_H

#include <libdragon.h>
#include <GL/gl.h>
#include <GL/glu.h>
#include <GL/gl_integration.h>
#include "glm/glm/glm.hpp"

#include "Camera.hpp"

class Engine
{
private:
    unsigned int chunkSize = 8;
    unsigned int chunkIndex = 0;

    float basePlayerSpeed = 10.0f;

    Camera player = Camera(0.0f, 0.0f, -2.0f);

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

    unsigned char chunk[3 * 3 * 3 * 27] = {};

    void update();
    void render();

    unsigned char pickBlock(unsigned char chunk[], unsigned char chunkSize, unsigned char x, unsigned char y, unsigned char z);
    void setBlock(unsigned char chunk[], unsigned char chunkSize, unsigned char blockSize, unsigned char x, unsigned char y, unsigned char z);

    void generateChunk(unsigned char chunk[], unsigned char chunkSize);
    void drawBlock(unsigned char chunk[], unsigned char chunkSize, unsigned char x, unsigned char y, unsigned char z);
    void drawChunk(unsigned char chunk[], unsigned char chunkSize);

public:
    Engine();
    void run();
};

#endif