#include "WindowHandler.h"

WindowHandler::WindowHandler(int w, int h) {
    lastMousePos[0] = INT_MIN; lastMousePos[1] = INT_MIN;
    this->massInit(w, h);
}

void WindowHandler::massInit(int w, int h) {
    SDL_Init(SDL_INIT_VIDEO);

    SDL_WindowFlags window_flags = (SDL_WindowFlags)(SDL_WINDOW_VULKAN | SDL_WINDOW_RESIZABLE);

    window = SDL_CreateWindow(
        "Test Space",
        SDL_WINDOWPOS_UNDEFINED,
        SDL_WINDOWPOS_UNDEFINED,
        w, h,
        window_flags
    );
}

void WindowHandler::clearMouseData() {
    this->mouseData[5] = 0;
    for (int i = 2; i < 5; i++) {
        if (mouseData[i] == 2) {
            mouseData[i] = 1;
        }
    }
}

void WindowHandler::clearKeyData() {
    for (int i = 0; i < KEYMAX; i++) {
        if (keyData[i] == 2) {
            keyData[i] = 1;
        }
    }
}

int WindowHandler::looper() { 
    clearMouseData();
    clearKeyData();

    int res = eventHandler();
    if (!res) return 0;

    moveDiff[0] = mouseData[0] - lastMousePos[0];
    moveDiff[1] = mouseData[1] - lastMousePos[1];

    lastMousePos[0] = mouseData[0];
    lastMousePos[1] = mouseData[1];

    return 1;
}

int WindowHandler::eventHandler() {
    SDL_Event cEvent; // current event
    while (SDL_PollEvent(&cEvent)) {
        switch (cEvent.type)
        {
        case SDL_QUIT:
            return 0;

        case SDL_MOUSEMOTION:
            mouseMovementEventCallback(cEvent.motion.x, cEvent.motion.y);
            break;

        case SDL_MOUSEBUTTONDOWN:
        case SDL_MOUSEBUTTONUP:
            mouseButtonEventCallback(cEvent.button.button, cEvent.button.type);
            break;

        case SDL_MOUSEWHEEL:
            scrollEventCallback(cEvent.wheel.x, cEvent.wheel.y);
            break;

        case SDL_KEYDOWN:
        case SDL_KEYUP:
            keyEventCallback(cEvent.key.keysym.scancode, 0, cEvent.key.type);
            break;

        case SDL_WINDOWEVENT:
            windowEventCallBack(cEvent);
            break;

        default:
            break;
        }
    }

    return 1;
}

void WindowHandler::mouseMovementEventCallback(double xpos, double ypos)
{
    this->mouseData[0] = (int)xpos;
    this->mouseData[1] = (int)ypos;
}

void WindowHandler::mouseButtonEventCallback(int button, int action) {
    switch (action)
    {
    case SDL_MOUSEBUTTONDOWN:
        this->mouseData[button + 1] = 2;
        break;

    case SDL_MOUSEBUTTONUP:
        this->mouseData[button + 1] = 0;
        break;

    default:
        break;
    }
}

void WindowHandler::scrollEventCallback(double xoffset, double yoffset) {
    this->mouseData[5] = (int)yoffset;
}

void WindowHandler::keyEventCallback(int key, int scancode, int action)
{
    switch (action)
    {
    case SDL_KEYDOWN:
        this->keyData[key] = 2;
        break;

    case SDL_KEYUP:
        this->keyData[key] = 0;
        break;

    default:
        break;
    }
}

void WindowHandler::windowEventCallBack(SDL_Event wEvent) {
    switch (wEvent.window.event)
    {
    case SDL_WINDOWEVENT_MINIMIZED:
        for (int i = 0; i < MOUSEMAX; i++) {
            mouseData[i] = 0;
        }

        for (int i = 0; i < KEYMAX; i++) {
            keyData[i] = 0;
        }

        break;

    default:
        break;
    }
}