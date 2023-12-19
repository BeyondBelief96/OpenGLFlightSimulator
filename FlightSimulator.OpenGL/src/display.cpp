#include <iostream>
#include "display.h"

// Callback function for when the framebuffer size changes.
void framebuffer_size_callback(GLFWwindow* window, int width, int height);

// Constructor: Sets up the GLFW window and its callbacks.
GameDisplay::GameDisplay(int width, int height, string windowTitle, GLFWcursorposfun mouseCallback, GLFWscrollfun scrollCallback)
{
    glfwInit(); // Initialize GLFW.
    // Set GLFW window hints for version and profile.
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    // Create a window with the specified dimensions and title.
    window = glfwCreateWindow(width, height, windowTitle.c_str(), NULL, NULL);
    if (window == NULL)
    {
        std::cout << "Failed to create GLFW window." << std::endl;
        glfwTerminate();
    }
    glfwMakeContextCurrent(window); // Set the window's context as current.

    // Set callback functions for window resize, mouse movement, and scrolling.
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    glfwSetCursorPosCallback(window, mouseCallback);
    glfwSetScrollCallback(window, scrollCallback);
}

// GetWidth: Retrieves and returns the current width of the GLFW window.
int GameDisplay::GetWidth()
{
    int width;
    glfwGetWindowSize(window, &width, NULL);
    return width;
}

// GetHeight: Retrieves and returns the current height of the GLFW window.
int GameDisplay::GetHeight()
{
    int height;
    glfwGetWindowSize(window, NULL, &height);
    return height;
}

// DeltaTime: Returns the time difference between the current and last frame.
float GameDisplay::DeltaTime()
{
    return deltaTime;
}

// GetWindow: Returns a pointer to the GLFW window context.
GLFWwindow* GameDisplay::GetWindow()
{
    return window;
}

// SetupFrameTime: Calculates and updates the time difference between frames.
void GameDisplay::SetupFrameTime()
{
    float currentFrame = static_cast<float>(glfwGetTime());
    deltaTime = currentFrame - lastFrame;
    lastFrame = currentFrame;
}

// Framebuffer Size Callback: Adjusts the viewport when the window size changes.
void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    glViewport(0, 0, width, height); // Adjust the OpenGL viewport to the new window size.
}
