#include <stdlib.h>

#include <glad/glad.h>
#include "window.h"
#include "shader.h"

#include <math.h>

#define TEX(tex) (GL_TEXTURE0 + tex)

#define CANVAS_TEX 0

typedef struct Canvas {
    GLuint draw_vao;
    GLuint draw_vbo;
    GLuint FBO;
    GLuint tex;
    pixelDim size;
    pixelPos off;
    pixelPos new_off;
    pixelPos pen;
    pixelPos prev_pen;
    float zoom;
} Canvas;

void genSquareVAO(float top, float btm, float lft, float rgt, GLuint *VAO, GLuint *VBO) {
    GLuint vbo_temp;
    if(VBO == NULL) {
        VBO = &vbo_temp;
    }

    float verticies[] = {
        rgt, top, // top right
        rgt, btm, // bottom right
        lft, btm, // bottom left
        lft, top, // top left 
    };

    uint indices[] = {
        0, 1, 3,   // first triangle
        1, 2, 3    // second triangle
    };

    // Generate a vertex array object.
    glGenVertexArrays(1, VAO);

    // Bind the vertex array object.
    glBindVertexArray(*VAO);

    // Generate buffer objects.
    GLuint EBO;
    glGenBuffers(1, VBO);
    glGenBuffers(1, &EBO);

    // Bind the virtual and element buffer objects.
    glBindBuffer(GL_ARRAY_BUFFER, *VBO);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);

    // Send the vertex and index data to the buffer objects.
    glBufferData(GL_ARRAY_BUFFER, sizeof(verticies), verticies, GL_STATIC_DRAW);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

    // Tell the shader how to interpret the VBO.
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), 0);

    // Enable the attributes.
    glEnableVertexAttribArray(0);
}

// Set up a vertex array object that the game board should be rendered on.
void createCanvasVAO(Canvas *canvas) {
    genSquareVAO(canvas->size.h, 0, 0, canvas->size.w, &canvas->draw_vao, &canvas->draw_vbo);
}

void createCanvasTex(Canvas *canvas) {
    glActiveTexture(TEX(CANVAS_TEX));
    glGenTextures(1, &canvas->tex);
    glBindTexture(GL_TEXTURE_2D, canvas->tex);

    uint32_t *pixels = (uint32_t *) malloc((size_t) canvas->size.w * canvas->size.h * 4);
    for(size_t i = 0; i < (size_t) canvas->size.w * canvas->size.h; i++) {
        pixels[i] = 0xFFFFFFFF;
    }

    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, canvas->size.w, canvas->size.h, 0, GL_RGBA, GL_UNSIGNED_BYTE, pixels);

    free(pixels);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
}

int createCanvasFBO(Canvas *canvas) {
    glGenFramebuffers(1, &canvas->FBO);
    glBindFramebuffer(GL_FRAMEBUFFER, canvas->FBO);

    createCanvasTex(canvas);

    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, canvas->tex, 0);

    if(glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
        printf("Framebuffer incomplete\n");
        return -1;
    }

    return 0;
}

void mouseToPixPos(pixelPosi *pos, int x, int y, int screen_height) {
    pos->x = 2 * x;
    pos->y = screen_height - (2 * y);
}

void calculatePenPos(Canvas *canvas, dispWindow *window) {
    canvas->pen.x = (window->mouse.x / canvas->zoom) - canvas->off.x;
    canvas->pen.y = (window->mouse.y / canvas->zoom) - canvas->off.y;
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
                SDL_GetWindowSizeInPixels(window->window, &window->size.w, &window->size.h);
                glViewport(0, 0, window->size.w, window->size.h);
                // Should keep the pen consistent or something similar (maybe middle of the screen)
            }
            break;
        case SDL_MOUSEWHEEL: {
            float a = event.wheel.y * 0.1;
            canvas->off.x = (window->mouse.x / (canvas->zoom + a)) - canvas->pen.x;
            canvas->off.y = (window->mouse.y / (canvas->zoom + a)) - canvas->pen.y;
            canvas->zoom += a;
            break;
        }
        case SDL_MOUSEMOTION:
            mouseToPixPos(&window->mouse, event.motion.x, event.motion.y, window->size.h);
            calculatePenPos(canvas, window);
            break;

        case SDL_MOUSEBUTTONUP:
            if(event.button.button == 1) {
                window->lmb_down = 0;
            } else if (event.button.button == 3) {
                window->rmb_down = 0;
                canvas->off.x += canvas->new_off.x;
                canvas->off.y += canvas->new_off.y;
                canvas->new_off.x = 0;
                canvas->new_off.y = 0;
                calculatePenPos(canvas, window);
            }
            break;
        case SDL_MOUSEBUTTONDOWN:
            if(event.button.button == 1) {
                window->lmb_down = 1;
                canvas->prev_pen.x = canvas->pen.x;
                canvas->prev_pen.y = canvas->pen.y;
            } else if (event.button.button == 3) {
                window->rmb_down = 1;
                mouseToPixPos(&window->rmb_down_pos, event.button.x, event.button.y, window->size.h);
            }
            break;
        default:
            break;
        }
    }
}

int runApp(dispWindow *window) {
    GLuint canvas_shader;
    if(buildShader(&canvas_shader, "../shaders/vertex.glsl", "../shaders/fragment.glsl") == -1) {
        return -1;
    }
    GLuint shader;
    if(buildShader(&shader, "../shaders/screenvert.glsl", "../shaders/screenfrag.glsl") == -1) {
        return -1;
    }

    Canvas canvas;
    canvas.size.w = 64;
    canvas.size.h = 64;
    canvas.off.x = (window->size.w - (int) canvas.size.w) / 2;
    canvas.off.y = (window->size.h - (int) canvas.size.h) / 2;
    canvas.new_off.x = 0;
    canvas.new_off.y = 0;
    canvas.zoom = 1;
    createCanvasVAO(&canvas);
    if(createCanvasFBO(&canvas) == -1) {
        return -1;
    }

    GLuint canvas_vao;
    genSquareVAO(canvas.size.h, 0, 0, canvas.size.w, &canvas_vao, NULL);

    glUseProgram(shader);
    glUniform1i(glGetUniformLocation(shader, "canvas_tex"), CANVAS_TEX);

    GLuint offset_loc = glGetUniformLocation(shader, "offset");
    GLuint screen_loc = glGetUniformLocation(shader, "screen_dim");
    GLuint zoom_loc = glGetUniformLocation(shader, "zoom");

    glUseProgram(canvas_shader);
    glUniform1i(glGetUniformLocation(canvas_shader, "canvas_tex"), CANVAS_TEX);

    GLuint mouse_loc = glGetUniformLocation(canvas_shader, "mouse_pos");
    GLuint prev_mouse_loc = glGetUniformLocation(canvas_shader, "prev_mouse_pos");
    GLuint pen_loc = glGetUniformLocation(canvas_shader, "pen_down");

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
        // Work out time between last frame and this one.
        uint64_t delta_time = time_value - prev_time;
        prev_time = time_value;

        glUseProgram(canvas_shader);
        if(window->lmb_down) {
            glUniform2i(prev_mouse_loc, canvas.prev_pen.x, canvas.prev_pen.y);
            glUniform2i(mouse_loc, canvas.pen.x, canvas.pen.y);
            canvas.prev_pen.x = canvas.pen.x;
            canvas.prev_pen.y = canvas.pen.y;
        }
        glUniform1i(pen_loc, window->lmb_down);

        glBindFramebuffer(GL_FRAMEBUFFER, canvas.FBO);
        glBindVertexArray(canvas.draw_vao);
        glViewport(0, 0,canvas.size.w, canvas.size.h);
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

        glUseProgram(shader);
        if(window->rmb_down) {
            canvas.new_off.x = (window->mouse.x - window->rmb_down_pos.x) / canvas.zoom;
            canvas.new_off.y = (window->mouse.y - window->rmb_down_pos.y) / canvas.zoom;
        }

        glUniform2f(offset_loc, canvas.off.x + canvas.new_off.x, canvas.off.y + canvas.new_off.y);
        glUniform2i(screen_loc, window->size.w, window->size.h);
        glUniform1f(zoom_loc, canvas.zoom);

        glBindFramebuffer(GL_FRAMEBUFFER, 0);
        glBindVertexArray(canvas_vao);
        glViewport(0, 0, window->size.w, window->size.h);
        glClear(GL_COLOR_BUFFER_BIT);
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

        swapWindow(window);
    }

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
