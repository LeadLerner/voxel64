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
void Engine::update()
{
    deltaTime = display_get_delta_time();
    joypad_poll();

    tappedButton = joypad_get_buttons_pressed(JOYPAD_PORT_1);
    heldButton = joypad_get_buttons_held(JOYPAD_PORT_1);
    controllerInputs = joypad_get_inputs(JOYPAD_PORT_1);

    // could just be my controller, but tappedButton is needed to register the inputs quicker.

    if (heldButton.c_up || tappedButton.c_up)
    {
        cameraPos += basePlayerSpeed * deltaTime * cameraFront;
    }

    if (heldButton.c_down || tappedButton.c_down)
    {
        cameraPos -= basePlayerSpeed * deltaTime * cameraFront;
    }

    if (heldButton.c_left || tappedButton.c_left)
    {
        cameraPos -= glm::normalize(glm::cross(cameraFront, cameraUp)) * basePlayerSpeed * deltaTime;
    }

    if (heldButton.c_right || tappedButton.c_right)
    {
        cameraPos += glm::normalize(glm::cross(cameraFront, cameraUp)) * basePlayerSpeed * deltaTime;
    }

    if (controllerInputs.stick_x >= joystickDeadzone || controllerInputs.stick_x <= -joystickDeadzone)
    {
        cameraDirection.y += controllerInputs.stick_x * deltaTime;
        if (cameraDirection.y > 360)
        {
            cameraDirection.y = 0;
        }
        if (cameraDirection.y < 0)
        {
            cameraDirection.y = 360;
        }
    }

    if (controllerInputs.stick_y >= joystickDeadzone || controllerInputs.stick_y <= -joystickDeadzone)
    {
        cameraDirection.x += controllerInputs.stick_y * deltaTime;
        if (cameraDirection.x > 360)
        {
            cameraDirection.x = 0;
        }
        if (cameraDirection.x < 0)
        {
            cameraDirection.x = 360;
        }
    }

    front.x = cos(glm::radians(cameraDirection.y)) * cos(glm::radians(cameraDirection.x));
    front.y = sin(glm::radians(cameraDirection.x));
    front.z = sin(glm::radians(cameraDirection.y)) * cos(glm::radians(cameraDirection.x));

    cameraFront = glm::normalize(front);

    cameraRight = glm::normalize(glm::cross(cameraFront, up));
    cameraUp = glm::normalize(glm::cross(cameraRight, cameraFront));
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

    gluLookAt(
        cameraPos.x, cameraPos.y, cameraPos.z,
        cameraPos.x + cameraFront.x, cameraPos.y + cameraFront.y, cameraPos.z + cameraFront.z,
        cameraUp.x, cameraUp.y, cameraUp.z
    );

    // glTranslatef(0.0f, 0.0f, -5.0f);

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