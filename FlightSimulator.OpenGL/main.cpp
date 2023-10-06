#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <iostream>
#include "display.h"


int main()
{
	GameDisplay gameDisplay(1920, 1080, "FlightSimulator");

	// glad: load all OpenGL function pointers
	// ---------------------------------------
	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
	{
		std::cout << "Failed to initialize GLAD" << std::endl;
		return -1;
	}

	// initialize input callbacks and settings
	glfwSetInputMode(gameDisplay.GetWindow(), GLFW_CURSOR, GLFW_CURSOR_NORMAL);


	while (!glfwWindowShouldClose(gameDisplay.GetWindow()))
	{
		// Initialize new frame times
		gameDisplay.SetupFrameTime();

		glClearColor(0.05f, 0.05f, 0.05f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		// glfw: swap buffers and poll IO events (keys pressed/released, mouse moved etc.)
		// -------------------------------------------------------------------------------
		glfwSwapBuffers(gameDisplay.GetWindow());
		glfwPollEvents();
	}

	glfwTerminate();
	return 0;
}