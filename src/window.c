#include "window.h"

#include <glad/glad.h>

void swapWindow(dispWindow *window) {
    SDL_GL_SwapWindow(window->window);
}

void closeWindow(dispWindow *window) {
    free(window);
    SDL_Quit();
}

void updateWindowSize(dispWindow *window) {
    int w, h;
    SDL_GetWindowSizeInPixels(window->window, &w, &h);
    setDim2(&window->size, w, h);
}

SDL_Window *createSDLWindow(const char *title, int width, int height) {
    // Init SDL.
    printf("Initing SDL.\n");
    if(SDL_Init(SDL_INIT_VIDEO) < 0) {
        printf("Could not init SDL: %s\n", SDL_GetError());
        return NULL;
    }

    // Set profile to core and version.
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, GL_MAJOR_VER);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, GL_MINOR_VER);

    // Create the SDL window.
    printf("Creating window.\n");
    SDL_Window *window = SDL_CreateWindow(
        title,                                          // Window title
        SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, // Center the x and y position on the screen.
        width, height,                                  // Set height and width.
        WINDOW_FLAGS                                    // (Flags) Open window useable with OpenGL context.
    );

    // Check that the window was created.
    if(!window) {
        printf("Failed to create window: %s\n", SDL_GetError());
        return NULL;
    }

    // Create a GL context.
    printf("Creating context.\n");
    SDL_GL_CreateContext(window);

    // Give glad the function loader specific to the OS.
    printf("Loading GL loader.\n");
    if(!gladLoadGLLoader((GLADloadproc) SDL_GL_GetProcAddress)) {
        printf("Failed to load GL\n");
        return NULL;
    }

    return window;
}

dispWindow *createWindow(const char *title, int width, int height) {
    dispWindow *window = malloc(sizeof(dispWindow));
    if(window == NULL) {
        printf("Couldn't allocate space for window.");
        return NULL;
    }

    window->window = createSDLWindow(title, width, height);
    if(window->window == NULL) {
        closeWindow(window);
        return NULL;
    }

    // Set viewport to the correct width and height.
    updateWindowSize(window);

    // Set clear colour to black.
    glClearColor(0.1, 0.2, 0.2, 1.0f);

    window->running = 1;
    window->lmb_down = 0;
    window->rmb_down = 0;

    // Return a pointer to the window.
    return window;
}