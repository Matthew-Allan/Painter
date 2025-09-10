#ifndef WINDOW_H
#define WINDOW_H

#define SDL_MAIN_HANDLED
#include <SDL2/SDL.h>

#define GL_MAJOR_VER 3  // The OpenGL major version number to target.
#define GL_MINOR_VER 3  // The OpenGL minor version number to target.

// Flags for when creating a window. Use OpenGL and allow high DPI.
#define WINDOW_FLAGS SDL_WINDOW_OPENGL | SDL_WINDOW_ALLOW_HIGHDPI | SDL_WINDOW_RESIZABLE

#define SCREEN_WIDTH 600    // Width of the screen in window points (not in pixels).
#define SCREEN_HEIGHT 600   // Height of the screen in window points (not in pixels).

// Set the target FPS.
#define fps 60// Create an SDL window and an OpenGL context.

typedef struct pixelPosi {
    int x;
    int y;
} pixelPosi;

typedef struct pixelPos {
    float x;
    float y;
} pixelPos;

typedef struct pixelDim {
    int w;
    int h;
} pixelDim;

// A struct containing window information.
typedef struct dispWindow {
    SDL_Window *window;
    pixelDim size;
    pixelPosi mouse;
    pixelPosi rmb_down_pos;
    uint running : 1;
    uint lmb_down : 1;
    uint rmb_down : 1;
} dispWindow;

dispWindow *createWindow(const char *title, int width, int height);
void closeWindow(dispWindow *window);

void swapWindow(dispWindow *window);

#endif