#pragma once
#ifndef DISPLAY_H
#define DISPLAY_H
#include <GLFW/glfw3.h>
#include <string>
using namespace std;

class GameDisplay
{
public:
	GameDisplay(int width, int height, string windowTitle);
	int GetWidth();
	int GetHeight();
	void SetupFrameTime();
	GLFWwindow* GetWindow();
private:

	// private members
	GLFWwindow* window;
	int width;
	int height;
	float deltaTime; // Time between current frame and last frame.
	float lastFrame; // Time of last frame.
};
#endif
