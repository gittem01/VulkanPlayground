#pragma once

#include <SDL.h>
#include <SDL_vulkan.h>
#include <vulkan/vulkan.h>
#include <glm/glm.hpp>
#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <string>

#define MOUSEMAX 6
#define KEYMAX 512
#define FPSCALCSIZE 10
#define REFRESH_INTERVAL 1000 // 1 sec

class WindowHandler
{
public:
	int* mouseData = (int*)calloc(MOUSEMAX, sizeof(int));
	int* keyData = (int*)calloc(KEYMAX, sizeof(int));

	int* lastMousePos = (int*)calloc(2, sizeof(int));
	int* moveDiff = (int*)calloc(2, sizeof(int));

	uint32_t* fpsArray = (uint32_t*)calloc(FPSCALCSIZE, sizeof(uint32_t));

	glm::vec2 lastWinPos, lastWinSize;

	SDL_Window* window;

	uint32_t deltaTimeMs;
	float deltaTimeSc;
	uint32_t fps;
	uint32_t frameNumber;
	VkExtent2D winExtent;

	WindowHandler(int w, int h);
	void init();

	void timeUpdate();

	int looper();

private:
	uint32_t totalFps;
	uint32_t lastTime;
	uint32_t titleTime;

	int eventHandler();
	void handleTime();
	void clearMouseData();
	void clearKeyData();
	void imRender(); // TODO
	void mouseMovementEventCallback(double, double);
	void mouseButtonEventCallback(int, int);
	void scrollEventCallback(double, double);
	void keyEventCallback(int key, int scancode, int action);
	void windowEventCallBack(SDL_Event wEvent);
	void windowSizeEventCallback(int, int);
};