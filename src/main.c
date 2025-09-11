#include <stdlib.h>

#include <math.h>

#include <glad/glad.h>
#include "core/window.h"
#include "core/shader.h"
#include "canvas.h"
#include "tex.h"

#define CANV_WIDTH 2000
#define CANV_HEIGHT 1200

typedef struct screenShader {
    GLuint prog;
    GLuint dim;
    GLuint off;
    GLuint zoom;
} screenShader;

void updateScreen(Canvas *canvas, dispWindow *window, screenShader *shader) {
    glUseProgram(shader->prog);
    if(window->rmb_down) {
        divVec2SI(subVec2(&canvas->new_off, &window->mouse, &window->rmb_down_pos), canvas->zoom);
        updateOff(canvas);
    }

    if(window->resized) {
        window->resized = 0;
        uniformIVec2(shader->dim, &window->size);
    }
    if(canvas->off_updt) {
        canvas->off_updt = 0;
        uniformVec2(shader->off, &canvas->off);
    }
    if(canvas->zoom_updt) {
        canvas->zoom_updt = 0;
        glUniform1f(shader->zoom, canvas->zoom);
    }

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glBindVertexArray(canvas->mesh.vao);
    glViewport(0, 0, window->size.w, window->size.h);
    glClearColor(0.1, 0.2, 0.2, 1.0);
    glClear(GL_COLOR_BUFFER_BIT);
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
}

// Check for any SDL events. Handles quitting and button presses. Returns if game should keep running.
void pollEvents(dispWindow *window, Canvas *canvas) {
    SDL_Event event;
    while(SDL_PollEvent(&event)) {
        switch (event.type) {
        case SDL_QUIT:
            window->running = 0;
            break;
        case SDL_WINDOWEVENT:
            if(event.window.event == SDL_WINDOWEVENT_RESIZED) {
                updateWindowSize(window);
            }
            break;
        case SDL_KEYDOWN:
            if(event.key.keysym.sym == SDLK_UP) {
                canvas->pen_size++;
                canvas->size_updt = 1;
            }
            if(event.key.keysym.sym == SDLK_DOWN) {
                canvas->pen_size--;
                canvas->size_updt = 1;
            }
            break;
        case SDL_MOUSEWHEEL:
            zoomCentered(canvas, window, &canvas->pen, &window->mouse, event.wheel.preciseY * 0.1);
            break;
        case SDL_MOUSEMOTION:
            mouseToVec2(&window->mouse, &window->pix_scale, event.motion.x, event.motion.y);
            if(!window->rmb_down) {
                calculatePenPos(canvas, window);
            }
            break;

        case SDL_MOUSEBUTTONUP:
            if(event.button.button == 1) {
                window->lmb_down = 0;
            } else if (event.button.button == 3) {
                window->rmb_down = 0;
                rstBaseOff(canvas);
                calculatePenPos(canvas, window);
            }
            break;
        case SDL_MOUSEBUTTONDOWN:
            if(event.button.button == 1) {
                window->lmb_down = 1;
                cpyVec2(&canvas->prev_pen, &canvas->pen);
            } else if (event.button.button == 3) {
                window->rmb_down = 1;
                mouseToVec2(&window->rmb_down_pos, &window->pix_scale, event.button.x, event.button.y);
            }
            break;
        default:
            break;
        }
    }
}

int runApp(dispWindow *window) {
    screenShader screen_shader;
    if(buildShader(&screen_shader.prog, "shaders/screen/vert.glsl", "shaders/screen/frag.glsl") == -1) {
        return -1;
    }

    glUseProgram(screen_shader.prog);
    glUniform1i(glGetUniformLocation(screen_shader.prog, "canvas_tex"), CANVAS_TEX);

    screen_shader.dim = glGetUniformLocation(screen_shader.prog, "screen_dim");
    screen_shader.off = glGetUniformLocation(screen_shader.prog, "offset");
    screen_shader.zoom = glGetUniformLocation(screen_shader.prog, "zoom");

    Canvas canvas;
    initCanvas(&canvas, window, CANV_WIDTH, CANV_HEIGHT);

    // Set the time in ms that the game starts.
    uint64_t prev_time = SDL_GetTicks64();
    while(window->running) {
        pollEvents(window, &canvas);

        // Loop until enough time has passed for the fps to be correct.
        uint64_t time_value;
        while((time_value = SDL_GetTicks64()) - prev_time < (1000 / fps)) {
            SDL_Delay(1);
            pollEvents(window, &canvas);
        }
        prev_time = time_value;

        if(window->lmb_down) {
            updateCanvas(&canvas);
        }

        updateScreen(&canvas, window, &screen_shader);

        swapWindow(window);
    }

    closeCanvas(&canvas);

    return 0;
}

int main(int argc, char const *argv[]) {
    dispWindow *window = createWindow("Painter", 1000, 600);
    if(window == NULL) {
        return -1;
    }
    runApp(window);
    closeWindow(window);
    return 0;
}
