#pragma once

#include <SDL.h>
#include <SDL_vulkan.h>
#include <vulkan/vulkan.h>
#include <glm/glm.hpp>
#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <string>

#include "imgui.h"
#include "backends/imgui_impl_sdl.h"
#include "backends/imgui_impl_vulkan.h"

class WindowHandler
{
public:
	SDL_Window* window;
	VkExtent2D winExtent;

	ImGuiIO* io;

	uint32_t frameNumber = 0;

	WindowHandler(int w, int h);
	void init();

	int looper();

private:
	int eventHandler();
};