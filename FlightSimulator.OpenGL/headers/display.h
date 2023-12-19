#pragma once
#ifndef DISPLAY_H
#define DISPLAY_H

#include <GLFW/glfw3.h>
#include <string>
using namespace std;

// GameDisplay: A class to manage the game's display window using GLFW.
class GameDisplay
{
public:
    GameDisplay() {} // Default empty constructor

    // Constructor: Initializes a new GameDisplay with specified dimensions, title, and callbacks.
    // width: The width of the window in pixels.
    // height: The height of the window in pixels.
    // windowTitle: The title of the window.
    // mouseCallback: Function pointer for mouse movement callback.
    // scrollCallback: Function pointer for scroll callback.
    GameDisplay(int width, int height, string windowTitle, GLFWcursorposfun mouseCallback, GLFWscrollfun scrollCallback);

    // GetWidth: Returns the current width of the window.
    int GetWidth();

    // GetHeight: Returns the current height of the window.
    int GetHeight();

    // SetupFrameTime: Updates frame timing information for smooth animations and transitions.
    void SetupFrameTime();

    // DeltaTime: Returns the time elapsed between the current and the last frame.
    float DeltaTime();

    // GetWindow: Returns a pointer to the GLFW window.
    GLFWwindow* GetWindow();

private:
    GLFWwindow* window; // The GLFW window context.
    int width;         // The width of the window.
    int height;        // The height of the window.
    float deltaTime;   // Time between the current and last frame.
    float lastFrame;   // Time at which the last frame was rendered.
};

#endif // DISPLAY_H
