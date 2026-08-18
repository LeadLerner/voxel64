// started off with a basic opengl example from https://n64squid.com/homebrew/libdragon/opengl/hello-triangle/
// This is going to be a basic voxel based game (such as minecraft) for the n64.

#include "Engine.hpp"

Engine::Engine()
{
    dfs_init(DFS_DEFAULT_LOCATION);
    joypad_init();
    display_init(RESOLUTION_640x480, DEPTH_16_BPP, 3, GAMMA_NONE, FILTERS_DISABLED);
    rdpq_init();
    gl_init();

    // Setup
    float aspect_ratio = (float)display_get_width() / (float)display_get_height();
    float near_plane = 1.0f;
    float far_plane = 50.0f;

    // Set the viewing area
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glFrustum(
        -near_plane * aspect_ratio,
        near_plane * aspect_ratio,
        -near_plane,
        near_plane,
        near_plane,
        far_plane);

    glEnable(GL_CULL_FACE);
    glEnable(GL_NORMALIZE);

    glEnableClientState(GL_VERTEX_ARRAY);

    glVertexPointer(3, GL_FLOAT, 0, blockVertices);
}

void Engine::generateChunk(unsigned char chunk[], unsigned char chunkSize)
{
    for (int i = 0; i < chunkSize * chunkSize * chunkSize; i++)
    {

        chunk[i] = 1;
    }
}

unsigned char Engine::pickBlock(unsigned char chunk[], unsigned char chunkSize, unsigned char x, unsigned char y, unsigned char z)
{
    return x + (y * chunkSize) + (z * chunkSize * chunkSize);
}

void Engine::drawChunk(unsigned char chunk[], unsigned char chunkSize)
{
    for (int x = 0; x < chunkSize; x++)
    {
        for (int y = 0; y < chunkSize; y++)
        {
            glTranslatef(0, y, 0);
            for (int z = 0; z < chunkSize; z++)
            {
                glTranslatef(0, 0, z);
                if (pickBlock(chunk, chunkSize, x, y, z) != 0)
                {
                    glColor4f(1.0f, 0.0f, 0.0f, 0.5f);
                    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_BYTE, blockBackIndices);
                    glColor4f(0.0f, 1.0f, 0.0f, 1.0f);
                    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_BYTE, blockFrontIndices);
                    glColor4f(0.0f, 0.0f, 1.0f, 1.0f);
                    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_BYTE, blockLeftIndices);
                    glColor4f(1.0f, 1.0f, 0.0f, 1.0f);
                    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_BYTE, blockRightIndices);
                    glColor4f(1.0f, 0.0f, 1.0f, 1.0f);
                    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_BYTE, blockBottomIndices);
                    glColor4f(0.0f, 1.0f, 1.0f, 1.0f);
                    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_BYTE, blockTopIndices);
                }
            }
        }
    }
}

void Engine::update()
{
    generateChunk(chunk, 3);
    deltaTime = display_get_delta_time();
    joypad_poll();

    tappedButton = joypad_get_buttons_pressed(JOYPAD_PORT_1);
    heldButton = joypad_get_buttons_held(JOYPAD_PORT_1);
    controllerInputs = joypad_get_inputs(JOYPAD_PORT_1);

    // could just be my controller, but tappedButton is needed to register the inputs quicker.

    if (heldButton.d_up || tappedButton.d_up)
    {
        player.moveForward(deltaTime, basePlayerSpeed);
    }

    if (heldButton.d_down || tappedButton.d_down)
    {
        player.moveBackward(deltaTime, basePlayerSpeed);
    }

    if (heldButton.d_left || tappedButton.d_left)
    {
        player.moveLeft(deltaTime, basePlayerSpeed);
    }

    if (heldButton.d_right || tappedButton.d_right)
    {
        player.moveRight(deltaTime, basePlayerSpeed);
    }

    if (controllerInputs.stick_x >= joystickDeadzone || controllerInputs.stick_x <= -joystickDeadzone)
    {
        player.lookYaw(deltaTime, controllerInputs.stick_x);
    }

    if (controllerInputs.stick_y >= joystickDeadzone || controllerInputs.stick_y <= -joystickDeadzone)
    {
        player.lookPitch(deltaTime, controllerInputs.stick_y);
    }
    player.update();
}

void Engine::render()
{
    // Start a new frame
    // Get the frame buffer and z-buffer
    surface_t *disp = display_get();
    surface_t *zbuf = display_get_zbuf();
    // Attach the buffers to the RDP
    rdpq_attach_clear(disp, zbuf);

    // Begin OpenGL compatibility with the RDP
    gl_context_begin();

    // Set the camera's position
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    player.moveAndOrient();

    drawChunk(chunk, 3);

    gl_context_end();

    // Send frame buffer to display (TV)
    rdpq_detach_show();
}

void Engine::run()
{
    while (1)
    {
        update();
        render();
    }
}