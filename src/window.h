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

// A struct containing window information.
typedef struct dispWindow {
    SDL_Window *window;
    int width;
    int height;
    int mouse_x;
    int mouse_y;
    int rmb_down_x;
    int rmb_down_y;
    uint running : 1;
    uint lmb_down : 1;
    uint rmb_down : 1;
} dispWindow;

dispWindow *createWindow(const char *title, int width, int height);
void closeWindow(dispWindow *window);

void swapWindow(dispWindow *window);

#endif