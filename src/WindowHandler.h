#pragma once

#include <SDL.h>
#include <SDL_vulkan.h>
#include <vulkan/vulkan.h>
#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <glm/glm.hpp>

#define MOUSEMAX 6
#define KEYMAX 512

class WindowHandler
{
public:
	int* mouseData = (int*)calloc(MOUSEMAX, sizeof(int));
	int* keyData = (int*)calloc(KEYMAX, sizeof(int));

	int* lastMousePos = (int*)calloc(2, sizeof(int));
	int* moveDiff = (int*)calloc(2, sizeof(int));
	glm::vec2 lastWinPos, lastWinSize;

	SDL_Window* window;

	WindowHandler(int w, int h);

	void massInit(int w, int h);
	int looper();
	int eventHandler();
	void clearMouseData();
	void clearKeyData();
	void imRender(); // TODO
	void mouseMovementEventCallback(double, double);
	void mouseButtonEventCallback(int, int);
	void scrollEventCallback(double, double);
	void keyEventCallback(int key, int scancode, int action);
	void windowEventCallBack(SDL_Event wEvent);
	void windowSizeEventCallback(int, int);

private:
	
};