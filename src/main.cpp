// started off with a basic opengl example from https://n64squid.com/homebrew/libdragon/opengl/hello-triangle/
// This is going to be a basic voxel based game (such as minecraft) for the n64.

#include <libdragon.h>
#include <GL/gl.h>
#include <GL/glu.h>
#include <GL/gl_integration.h>

float playerX = 0.0f;
float playerY = 0.0f;
float playerZ = 0.0f;

float playerDirX = 0.0f;
float playerDirY = 0.0f;

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

void init()
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

void update()
{
	deltaTime = display_get_delta_time();
	joypad_poll();

	tappedButton = joypad_get_buttons_pressed(JOYPAD_PORT_1);
	heldButton = joypad_get_buttons_held(JOYPAD_PORT_1);
	controllerInputs = joypad_get_inputs(JOYPAD_PORT_1);

	// could just be my controller, but tappedButton is needed to register the inputs quicker. Probably going to switch the stick to rotation and the cstick for movement later.

	if (heldButton.c_left || tappedButton.c_left)
	{
		playerX += 10.0f * deltaTime;
	}

	if (heldButton.c_right || tappedButton.c_right)
	{
		playerX -= 10.0f * deltaTime;
	}

	if (heldButton.c_up || tappedButton.c_up)
	{
		playerZ += 10.0f * deltaTime;
	}

	if (heldButton.c_down || tappedButton.c_down)
	{
		playerZ -= 10.0f * deltaTime;
	}

	if (controllerInputs.stick_x >= joystickDeadzone || controllerInputs.stick_x <= -joystickDeadzone)
	{
		playerDirY -= controllerInputs.stick_x * deltaTime;
	}
}

void render()
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

	glTranslatef(0.0f, 0.0f, -5.0f);
	glRotatef(playerDirY, 0.0f, 1.0f, 0.0f);

	glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_BYTE, blockBackIndices);
	glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_BYTE, blockFrontIndices);

	gl_context_end();

	// Send frame buffer to display (TV)
	rdpq_detach_show();
}

int main(void)
{
	init();

	while (1)
	{
		update();
		render();
	}
}
