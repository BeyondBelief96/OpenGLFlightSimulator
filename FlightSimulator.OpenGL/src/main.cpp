#include <stb_image/stb_image.h>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <vector>
#include <glm/gtc/matrix_transform.hpp>
#include <iostream>

#include "display.h"
#include "model.h"
#include "camera.h"
#include "skybox.h"
#include "data.h"
#include "physics.h"
#include "terrain.h"
#include "fault_formation_terrain.h"

#include "constants.h"
#include "joystick.h"

// Callback function declarations
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
void processInput(GLFWwindow* window); 

// Global variables
GameDisplay gameDisplay(1920, 1080, "FlightSimulator", mouse_callback, scroll_callback);
float lastX = gameDisplay.GetWidth() / 2.0f;
float lastY = gameDisplay.GetHeight() / 2.0f;
bool firstMouse = true;
bool gamepadConnected;
Joystick joystick;
glm::vec3 initial_position = { 1500.0f, 5000.0f, 1000.0f};
Camera aircraftCamera = Camera(gameDisplay, initial_position);
Model planeModel;
FaultFormationTerrain m_terrain;

// Terrain variables
float worldScale = 20.0;
float textureScale = 40.0;
int terrainSize = 2048;
int iterations = 500;
float minHeight = 0.0f;
float maxHeight = 5000.0f;
float filter = 0.80f;

// Function declartions
void InitializeOpenGLState();
void RenderScene();
FaultFormationTerrain InitializeTerrain(float minHeight, float maxHeight);
FaultFormationTerrain InitTerrainMultiTextures(FaultFormationTerrain& terrain, float minHeight, float maxHeight);

// Main Application
int main()
{
	InitializeOpenGLState();

	// Initializes the terrain generation.
	m_terrain = InitializeTerrain(minHeight, maxHeight);

	// load models
	// -----------
	planeModel = Model(planeModelPath, planeModelVertexShaderPath, planeModelFragmentShaderPath);

	while (!glfwWindowShouldClose(gameDisplay.GetWindow()))
	{
		// Initialize new frame times
		gameDisplay.SetupFrameTime();

		// input
		processInput(gameDisplay.GetWindow());

		/*plane.update(gameDisplay.DeltaTime());*/

		RenderScene();
	}

	glfwTerminate();
	return 0;
}

void RenderScene()
{
	glClearColor(0.05f, 0.05f, 0.05f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	// view/projection transformations
	glm::mat4 viewProjMatrix = aircraftCamera.GetViewProjMatrix();

	// render the loaded model
	glm::mat4 planeModelMatrix = glm::mat4(1.0f);
	planeModelMatrix = glm::translate(planeModelMatrix, initial_position);
	planeModelMatrix = glm::scale(planeModelMatrix, glm::vec3(0.5f, 0.5f, 0.5f));
	planeModelMatrix = glm::rotate(planeModelMatrix, glm::radians(-90.0f), physics::UP);
	planeModel.Render(planeModelMatrix, aircraftCamera);

	m_terrain.Render(aircraftCamera);


	glBindVertexArray(0);

	// glfw: swap buffers and poll IO events (keys pressed/released, mouse moved etc.)
	// -------------------------------------------------------------------------------
	glfwSwapBuffers(gameDisplay.GetWindow());
	glfwPollEvents();
}

void InitializeOpenGLState()
{
	// OpenGL initialization
	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
	{
		std::cout << "Failed to initialize GLAD" << std::endl;
		exit(-1);
	}
	glfwSetInputMode(gameDisplay.GetWindow(), GLFW_CURSOR, GLFW_CURSOR_DISABLED);
}

FaultFormationTerrain InitializeTerrain(float minHeight, float maxHeight)
{
	FaultFormationTerrain terrain(terrainVertexShaderPath, terrainFragmentShaderPath);
	FaultFormationTerrain m_terrain = InitTerrainMultiTextures(terrain, minHeight, maxHeight);
	return terrain;
}

FaultFormationTerrain InitTerrainMultiTextures(FaultFormationTerrain& terrain, float minHeight, float maxHeight)
{
	std::vector<string> textureFileNames;
	textureFileNames.push_back(terrainTexture1Path);
	textureFileNames.push_back(terrainTexture2Path);
	textureFileNames.push_back(terrainTexture3Path);
	textureFileNames.push_back(terrainTexture4Path);
	terrain.InitTerrain(worldScale, textureScale, minHeight, maxHeight, textureFileNames);
	terrain.CreateFaultFormation(terrainSize, iterations, minHeight, maxHeight, filter);
	return terrain;
}

// process all input: query GLFW whether relevant keys are pressed/released this frame and react accordingly
// ---------------------------------------------------------------------------------------------------------
void processInput(GLFWwindow* window)
{
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
		glfwSetWindowShouldClose(window, true);

	if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
		aircraftCamera.ProcessKeyboard(FORWARD, gameDisplay.DeltaTime());
	if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
		aircraftCamera.ProcessKeyboard(BACKWARD, gameDisplay.DeltaTime());
	if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
		aircraftCamera.ProcessKeyboard(LEFT, gameDisplay.DeltaTime());
	if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
		aircraftCamera.ProcessKeyboard(RIGHT, gameDisplay.DeltaTime());

	/*if (glfwJoystickPresent(GLFW_JOYSTICK_1) && glfwJoystickIsGamepad(GLFW_JOYSTICK_1))
	{
		GLFWgamepadstate gamepadState;
		if (glfwGetGamepadState(GLFW_JOYSTICK_1, &gamepadState))
		{
			float leftAxisX = 0.0f, leftAxisY = 0.0f;;

			leftAxisX = gamepadState.axes[GLFW_GAMEPAD_AXIS_LEFT_X];
			leftAxisY = gamepadState.axes[GLFW_GAMEPAD_AXIS_LEFT_Y];

			if (leftAxisX < 0)
				joystick.leftAileron = leftAxisX;
			else
				joystick.rightAileron = leftAxisX;
			joystick.elevator = leftAxisY;
		}
	}*/
}

// glfw: whenever the mouse moves, this callback is called
// -------------------------------------------------------
void mouse_callback(GLFWwindow* window, double xposIn, double yposIn)
{
	float xpos = static_cast<float>(xposIn);
	float ypos = static_cast<float>(yposIn);

	if (firstMouse)
	{
		lastX = xpos;
		lastY = ypos;
		firstMouse = false;
	}

	float xoffset = xpos - lastX;
	float yoffset = lastY - ypos; // reversed since y-coordinates go from bottom to top

	lastX = xpos;
	lastY = ypos;

	aircraftCamera.ProcessMouseMovement(xoffset, yoffset);
}

// glfw: whenever the mouse scroll wheel scrolls, this callback is called
// ----------------------------------------------------------------------
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
	/*aircraftCamera.ProcessMouseScroll(static_cast<float>(yoffset));*/
}
