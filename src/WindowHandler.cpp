#include "WindowHandler.h"

WindowHandler::WindowHandler(int w, int h) {
    this->winExtent.width = w; this->winExtent.height = h;
    this->window = NULL;
    this->io = NULL;
}

void WindowHandler::init() {
    SDL_Init(SDL_INIT_VIDEO);

    SDL_WindowFlags window_flags = (SDL_WindowFlags)(SDL_WINDOW_VULKAN | SDL_WINDOW_RESIZABLE);
    
    window = SDL_CreateWindow(
        "Window",
        SDL_WINDOWPOS_UNDEFINED,
        SDL_WINDOWPOS_UNDEFINED,
        winExtent.width, winExtent.height,
        window_flags
    );
}

int WindowHandler::looper() {
    int res = eventHandler();
    if (!res) return 0;

    ImGui_ImplVulkan_NewFrame();
    ImGui_ImplSDL2_NewFrame();
    ImGui::NewFrame();
    
    return 1;
}


int WindowHandler::eventHandler() {
    SDL_Event cEvent; // current event
    
    while (SDL_PollEvent(&cEvent)) {
        if (cEvent.type == SDL_QUIT) return 0;
        ImGui_ImplSDL2_ProcessEvent(&cEvent);
    }

    return 1;
}