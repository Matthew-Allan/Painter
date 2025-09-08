#include <stdlib.h>
#include <glad/glad.h>

#include "window.h"

#include <math.h>

// Check for any SDL events. Handles quitting and button presses. Returns if game should keep running.
int pollEvents(dispWindow *window) {
    SDL_Event event;
    while(SDL_PollEvent(&event)) {
        switch (event.type) {
        case SDL_QUIT:
            return 0;
        case SDL_WINDOWEVENT:
            if(event.window.event == SDL_WINDOWEVENT_RESIZED) {
                SDL_GetWindowSizeInPixels(window->window, &window->width, &window->height);
                glViewport(0, 0, window->width, window->height);
            }
            break;
        default:
            break;   
        }
    }
    return 1;
}

int main(int argc, char const *argv[]) {
    dispWindow *window = createWindow("Painter", 1000, 600);
    if(window == NULL) {
        return -1;
    }
    while(pollEvents(window)) {
        glClear(GL_COLOR_BUFFER_BIT);
        swapWindow(window);
    }
    return 0;
}
