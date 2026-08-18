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
    glEnable(GL_DEPTH_TEST);

    glEnableClientState(GL_VERTEX_ARRAY);

    glVertexPointer(3, GL_FLOAT, 0, blockVertices);
}

void Engine::generateChunk(unsigned char chunk[], unsigned char chunkSize)
{
    // the name says it all. This just fiils the chunk with type 1 blocks. Will change soon.
    for (int i = 0; i < chunkSize * chunkSize * chunkSize; i++)
    {

        chunk[i] = 1;
    }
    setBlock(chunk, chunkSize, 0, 3, chunkSize - 1, 3);
}

void Engine::setBlock(unsigned char chunk[], unsigned char chunkSize, unsigned char blockType, unsigned char x, unsigned char y, unsigned char z)
{
    chunk[x + (y * chunkSize) + (z * chunkSize * chunkSize)] = blockType;
}

unsigned char Engine::pickBlock(unsigned char chunk[], unsigned char chunkSize, unsigned char x, unsigned char y, unsigned char z)
{
    // I think this works. Converts coords into 1d index.
    return chunk[x + (y * chunkSize) + (z * chunkSize * chunkSize)];
}

void Engine::drawBlock(unsigned char chunk[], unsigned char chunkSize, unsigned char x, unsigned char y, unsigned char z)
{
    glColor4f(1.0f, 0.0f, 0.0f, 0.5f);
    if (pickBlock(chunk, chunkSize, x, y, z - 1) == 0 || z == 0)
    {
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_BYTE, blockBackIndices);
    }
    glColor4f(0.0f, 1.0f, 0.0f, 1.0f);
    if (pickBlock(chunk, chunkSize, x, y, z + 1) == 0 || z == chunkSize - 1)
    {
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_BYTE, blockFrontIndices);
    }
    glColor4f(0.0f, 0.0f, 1.0f, 1.0f);
    if (pickBlock(chunk, chunkSize, x - 1, y, z) == 0 || x == 0)
    {
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_BYTE, blockLeftIndices);
    }
    glColor4f(1.0f, 1.0f, 0.0f, 1.0f);
    if (pickBlock(chunk, chunkSize, x + 1, y, z) == 0 || x == chunkSize - 1)
    {
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_BYTE, blockRightIndices);
    }
    glColor4f(1.0f, 0.0f, 1.0f, 0.5f);
    if (pickBlock(chunk, chunkSize, x, y - 1, z) == 0 || y == 0)
    {
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_BYTE, blockBottomIndices);
    }

    glColor4f(0.0f, 1.0f, 1.0f, 1.0f);
    if (pickBlock(chunk, chunkSize, x, y + 1, z) == 0 || y == chunkSize - 1)
    {
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_BYTE, blockTopIndices);
    }
}

void Engine::drawChunk(unsigned char chunk[], unsigned char chunkSize)
{
    // again, pretty self explanatory. Loop through the chunk (O(N^3) complexity but we'll deal with that later), check if it's air(block type 0), if it is, skip drawing.
    // if not air, draw the cube.
    for (int x = 0; x < chunkSize; x++)
    {
        glTranslatef(x, 0, 0);
        for (int y = 0; y < chunkSize; y++)
        {
            glTranslatef(0, y, 0);
            for (int z = 0; z < chunkSize; z++)
            {
                glTranslatef(0, 0, z);
                chunkIndex = pickBlock(chunk, chunkSize, x, y, z);
                if (chunkIndex != 0)
                {
                    drawBlock(chunk, chunkSize, x, y, z);
                }
                glTranslatef(0, 0, -z);
            }
            glTranslatef(0, -y, 0);
        }
        glTranslatef(-x, 0, 0);
    }
}

void Engine::update()
{
    // basically, generate the chunk each frame
    // not efficient, will change later to only generate once and regenerate when block broken, block placed, or entering a new chunk.

    // dpad moves playerCamera, joystick rotates it.
    generateChunk(chunk, chunkSize);
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
    // later on we will have a player class, so this will become player.camera.update() or something like that.
    player.update();
}

void Engine::render()
{
    // pretty simple, just grabs the display buffer and zbuffer, clears the screen, and draws a new frame.
    surface_t *disp = display_get();
    surface_t *zbuf = display_get_zbuf();
    // Attach the buffers to the RDP
    rdpq_attach_clear(disp, zbuf);

    gl_context_begin();

    // Set the camera's position
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    // perhaps this should be changed to player.draw(). Not accurate, but i need player movement inside a valid opengl context.
    player.moveAndOrient();

    drawChunk(chunk, chunkSize);

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