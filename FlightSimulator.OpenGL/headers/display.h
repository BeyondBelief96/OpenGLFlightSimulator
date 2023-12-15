#pragma once
#ifndef DISPLAY_H
#define DISPLAY_H
#include <GLFW/glfw3.h>
#include <string>
using namespace std;

class GameDisplay
{
public:
	GameDisplay(int width, int height, string windowTitle, GLFWcursorposfun mouseCallback, GLFWscrollfun scrollCallback);
	int GetWidth();
	int GetHeight();
	void SetupFrameTime();
	float DeltaTime();
	GLFWwindow* GetWindow();
private:
	GLFWwindow* window; // the open GL window context.
	int width; // window width
	int height; // window height
	float deltaTime; // Time between current frame and last frame.
	float lastFrame; // Time of last frame.
};
#endif