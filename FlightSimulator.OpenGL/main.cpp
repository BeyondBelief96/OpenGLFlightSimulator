#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <iostream>
#include "stb_image.h"
#include "display.h"
#include "model.h"
#include "camera.h"

int main()
{
	GameDisplay gameDisplay(1920, 1080, "FlightSimulator");

	// Camera Setup
	Camera camera(glm::vec3(0.0f, 0.0f, 0.0f));

	// glad: load all OpenGL function pointers
	// ---------------------------------------
	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
	{
		std::cout << "Failed to initialize GLAD" << std::endl;
		return -1;
	}

	// initialize input callbacks and settings
	glfwSetInputMode(gameDisplay.GetWindow(), GLFW_CURSOR, GLFW_CURSOR_NORMAL);

	// tell stb_image.h to flip loaded texture's on the y-axis (before loading model).
	stbi_set_flip_vertically_on_load(true);

	// build and compile our shader program
	// ------------------------------------
	Shader shader("./Shaders/modelLoading.vs", "./Shaders/modelLoading.fs");

	// load models
	// -----------
	const GLchar* modelPath = "./Models/nanosuit.obj";
	Model ourModel(modelPath);

	while (!glfwWindowShouldClose(gameDisplay.GetWindow()))
	{
		// Initialize new frame times
		gameDisplay.SetupFrameTime();

		glClearColor(0.05f, 0.05f, 0.05f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		// don't forget to enable shader before setting uniforms
		shader.Use();

		// view/projection transformations
		glm::mat4 projection = glm::perspective(glm::radians(camera.Zoom), (float)gameDisplay.GetWidth() / (float)gameDisplay.GetHeight(), 0.1f, 100.0f);
		glm::mat4 view = camera.GetViewMatrix();
		shader.setMat4("projection", projection);
		shader.setMat4("view", view);

		// glfw: swap buffers and poll IO events (keys pressed/released, mouse moved etc.)
		// -------------------------------------------------------------------------------
		glfwSwapBuffers(gameDisplay.GetWindow());
		glfwPollEvents();
	}

	glfwTerminate();
	return 0;
}