#pragma once

#include <SDL.h>
#include <SDL_vulkan.h>
#include <vulkan/vulkan.h>
#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <glm/glm.hpp>

class WindowHandler
{
public:
	int* mouseData = (int*)calloc(6, sizeof(int));
	int* keyData = (int*)calloc(512, sizeof(int));

	int* lastMousePos = (int*)calloc(2, sizeof(int));
	int* moveDiff = (int*)calloc(2, sizeof(int));
	glm::vec2 lastWinPos, lastWinSize;

	SDL_Window* window;

	int current_pos = 0;

	WindowHandler(int w, int h);

	void massInit(int w, int h);
	int looper();
	void clearMouseData();
	void clearKeyData();
	void imRender(); // TODO
	void mouseMovementEventCallback(double, double);
	void mouseButtonEventCallback(int, int);
	void scrollEventCallback(double, double);
	void keyEventCallback(int key, int scancode, int action);
	void windowSizeEventCallback(int, int);

private:
	
};