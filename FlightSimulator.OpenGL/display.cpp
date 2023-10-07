#include <iostream>
#include "display.h"

void framebuffer_size_callback(GLFWwindow* window, int width, int height);

GameDisplay::GameDisplay(int width, int height, string windowTitle, GLFWcursorposfun mouseCallback, GLFWscrollfun scrollCallback)
{
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	window = glfwCreateWindow(width, height, windowTitle.c_str(), NULL, NULL);
	if (window == NULL)
	{
		std::cout << "Failed to create GLFW window." << std::endl;
		glfwTerminate();
	}
	glfwMakeContextCurrent(window);
	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
	glfwSetCursorPosCallback(window, mouseCallback);
	glfwSetScrollCallback(window, scrollCallback);

	// configure global opengl state
	// -----------------------------
	glEnable(GL_DEPTH_TEST);
}

int GameDisplay::GetWidth()
{
	int width;
	glfwGetWindowSize(window, &width, NULL);
	return width;
}

int GameDisplay::GetHeight()
{
	int height;
	glfwGetWindowSize(window, NULL, &height);
	return height;
}

float GameDisplay::DeltaTime()
{
	return deltaTime;
}

GLFWwindow* GameDisplay::GetWindow()
{
	return window;
}


void GameDisplay::SetupFrameTime()
{
	float currentFrame = static_cast<float>(glfwGetTime());
	deltaTime = currentFrame - lastFrame;
	lastFrame = currentFrame;
}
// glfw: whenever the window size changed (by OS or user resize) this callback function executes
// ---------------------------------------------------------------------------------------------
void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
	// make sure the viewport matches the new window dimensions; note that width and 
	// height will be significantly larger than specified on retina displays.
	glViewport(0, 0, width, height);
}
