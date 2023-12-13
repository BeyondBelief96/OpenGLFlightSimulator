#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <vector>
#include <glm/gtc/matrix_transform.hpp>
#include <iostream>
#include <stb_image/stb_image.h>

#include "display.h"
#include "model.h"
#include "camera.h"
#include "texture.h"
#include "skybox.h"
#include "data.h"
#include "wing.h"
#include "physics.h"
#include "airplane.h"
#include "terrain.h"
#include "constants.h"

// Callback function declarations
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
void processInput(GLFWwindow* window);

// Global variables
GameDisplay gameDisplay(1920, 1080, "FlightSimulator", mouse_callback, scroll_callback);
float lastX = gameDisplay.GetWidth() / 2.0f;
float lastY = gameDisplay.GetHeight() / 2.0f;
bool firstMouse = true;

// Function to initialize OpenGL state, shaders, models, etc.
void initializeOpenGLState();

// airplane setup
const float mass = 1160; // Maximum ramp weight for C-172K in kg
const float thrust = 213; // Thrust of C-172K 

const float wing_offset = -1.0f;
const float tail_offset = -6.6f;

const Airfoil NACA_2412(NACA_2412_data);

std::vector<Wing> wings = {
	Wing({wing_offset, 0.0f, -2.7f}, 6.96f, 2.50f,& NACA_2412, physics::UP),         // left wing
	Wing({wing_offset - 1.5f, 0.0f, -2.0f}, 3.80f, 1.26f,& NACA_2412, physics::UP),    // left aileron
	Wing({wing_offset - 1.5f, 0.0f, 2.0f}, 3.80f, 1.26f,& NACA_2412, physics::UP),       // right aileron
	Wing({wing_offset, 0.0f, +2.7f}, 6.96f, 2.50f,& NACA_2412, physics::UP),             // right wing
	Wing({tail_offset, -0.1f, 0.0f}, 6.54f, 2.70f,& NACA_2412, physics::UP),             // elevator
	Wing({tail_offset, 0.0f, 0.0f}, 5.31f, 3.10f,& NACA_2412, physics::RIGHT),  // rudder
};

glm::mat3 inertia = {
	48531.0f,-1320.0f, 0.0f,
   -1320.0f, 256608.0f, 0.0f,
	0.0f, 0.0f, 211333.0f
};

Airplane plane = Airplane(mass, thrust, inertia, wings);

//Initializing camera
Camera aircraftCamera = Camera(gameDisplay, plane.position);

int main()
{
	initializeOpenGLState();

	//Load terrain
	BaseTerrain m_terrain(terrainVertexShaderPath, terrainFragmentShaderPath, 4.0f);
	m_terrain.LoadFromFile(heightMapFilePath);

	// load models
	// -----------
	Model planeModel(planeModelPath, planeModelVertexShaderPath, planeModelFragmentShaderPath);

	plane.position = { 0.0f, 300.0f, 0.0f };
	plane.velocity = { 1.0f, 0.0f, 0.0f };
	plane.orientation = glm::quat(1.0f, 0.0f, 0.0f, 0.0f);
	glm::vec3 aircraftFront = physics::FORWARD;
	glm::vec3 aircraftUp = physics::UP;

	GLuint i = 0;
	while (!glfwWindowShouldClose(gameDisplay.GetWindow()))
	{
		// Initialize new frame times
		gameDisplay.SetupFrameTime();

		// input
		// -----
		processInput(gameDisplay.GetWindow());

		/*plane.update(gameDisplay.DeltaTime());*/

		aircraftCamera.UpdateFromAircraft(plane.position, aircraftFront, aircraftUp, 0);

		glClearColor(0.05f, 0.05f, 0.05f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		// view/projection transformations
		glm::mat4 viewProjMatrix = aircraftCamera.GetViewProjMatrix();

		// render the loaded model
		glm::mat4 planeModelMatrix = glm::mat4(1.0f);
		planeModelMatrix = glm::translate(planeModelMatrix, plane.position); 
		planeModelMatrix = glm::scale(planeModelMatrix, glm::vec3(1.0f, 1.0f, 1.0f));
		planeModelMatrix = glm::rotate(planeModelMatrix, glm::radians(180.0f), glm::normalize(aircraftCamera.WorldUp));
		planeModel.Draw(planeModelMatrix, aircraftCamera);

		m_terrain.Render(aircraftCamera);

		glBindVertexArray(0);

		// glfw: swap buffers and poll IO events (keys pressed/released, mouse moved etc.)
		// -------------------------------------------------------------------------------
		glfwSwapBuffers(gameDisplay.GetWindow());
		glfwPollEvents();
		i++;
	}

	glfwTerminate();
	return 0;
}

void initializeOpenGLState()
{
	// OpenGL initialization
	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
	{
		std::cout << "Failed to initialize GLAD" << std::endl;
		exit(-1);
	}
	glfwSetInputMode(gameDisplay.GetWindow(), GLFW_CURSOR, GLFW_CURSOR_DISABLED);
}

// process all input: query GLFW whether relevant keys are pressed/released this frame and react accordingly
// ---------------------------------------------------------------------------------------------------------
void processInput(GLFWwindow* window)
{
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
		glfwSetWindowShouldClose(window, true);

	/*if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
		aircraftCamera.ProcessKeyboard(FORWARD, gameDisplay.DeltaTime());
	if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
		aircraftCamera.ProcessKeyboard(BACKWARD, gameDisplay.DeltaTime());
	if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
		aircraftCamera.ProcessKeyboard(LEFT, gameDisplay.DeltaTime());
	if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
		aircraftCamera.ProcessKeyboard(RIGHT, gameDisplay.DeltaTime());*/
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
	aircraftCamera.ProcessMouseScroll(static_cast<float>(yoffset));
}
