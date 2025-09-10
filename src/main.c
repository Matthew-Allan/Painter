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
    uint width;
    uint height;
    int off_x;
    int off_y;
    int new_off_x;
    int new_off_y;
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
    genSquareVAO(canvas->height, 0, 0, canvas->width, &canvas->draw_vao, &canvas->draw_vbo);
}

void createCanvasTex(Canvas *canvas) {
    glActiveTexture(TEX(CANVAS_TEX));
    glGenTextures(1, &canvas->tex);
    glBindTexture(GL_TEXTURE_2D, canvas->tex);

    uint32_t *pixels = (uint32_t *) malloc((size_t) canvas->width * canvas->height * 4);
    for(size_t i = 0; i < (size_t) canvas->width * canvas->height; i++) {
        pixels[i] = 0xFFFFFFFF;
    }

    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, canvas->width, canvas->height, 0, GL_RGBA, GL_UNSIGNED_BYTE, pixels);

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
                SDL_GetWindowSizeInPixels(window->window, &window->width, &window->height);
                glViewport(0, 0, window->width, window->height);
            }
            break;
        case SDL_MOUSEMOTION:
            window->mouse_x = event.motion.x;
            window->mouse_y = event.motion.y;
            break;

        case SDL_MOUSEBUTTONUP:
            if(event.button.button == 1) {
                window->lmb_down = 0;
            } else if (event.button.button == 3) {
                window->rmb_down = 0;
                canvas->off_x += canvas->new_off_x;
                canvas->off_y += canvas->new_off_y;
            }
            break;
        case SDL_MOUSEBUTTONDOWN:
            if(event.button.button == 1) {
                window->lmb_down = 1;
                window->prev_mouse_x = event.button.x;
                window->prev_mouse_y = event.button.y;
            } else if (event.button.button == 3) {
                window->rmb_down = 1;
                window->rmb_down_x = event.button.x;
                window->rmb_down_y = event.button.y;
                canvas->new_off_x = 0;
                canvas->new_off_y = 0;
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
    canvas.width = 1800;
    canvas.height = 1000;
    canvas.off_x = (window->width - (int) canvas.width) / 2;
    canvas.off_y = (window->height - (int) canvas.height) / 2;
    createCanvasVAO(&canvas);
    if(createCanvasFBO(&canvas) == -1) {
        return -1;
    }

    GLuint canvas_vao;
    genSquareVAO(canvas.height, 0, 0, canvas.width, &canvas_vao, NULL);

    glUseProgram(shader);
    glUniform1i(glGetUniformLocation(shader, "canvas_tex"), CANVAS_TEX);

    GLuint offset_loc = glGetUniformLocation(shader, "offset");
    GLuint screen_loc = glGetUniformLocation(shader, "screen_dim");

    glUniform2i(offset_loc, canvas.off_x, canvas.off_y);

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
            glUniform2i(prev_mouse_loc, (2 * window->prev_mouse_x) - canvas.off_x, (window->height - (2 * window->prev_mouse_y)) - canvas.off_y);
            glUniform2i(mouse_loc, (2 * window->mouse_x) - canvas.off_x, (window->height - (2 * window->mouse_y)) - canvas.off_y);
            window->prev_mouse_x = window->mouse_x;
            window->prev_mouse_y = window->mouse_y;
        }
        glUniform1i(pen_loc, window->lmb_down);

        glBindFramebuffer(GL_FRAMEBUFFER, canvas.FBO);
        glBindVertexArray(canvas.draw_vao);
        glViewport(0, 0,canvas.width, canvas.height);
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

        glUseProgram(shader);
        if(window->rmb_down) {
            canvas.new_off_x = 2 * (window->mouse_x - window->rmb_down_x);
            canvas.new_off_y = 2 * (window->rmb_down_y - window->mouse_y);
            int offset_x = canvas.off_x + canvas.new_off_x;
            int offset_y = canvas.off_y + canvas.new_off_y;
            glUniform2i(offset_loc, offset_x, offset_y);
        }
        glUniform2i(screen_loc, window->width, window->height);

        glBindFramebuffer(GL_FRAMEBUFFER, 0);
        glBindVertexArray(canvas_vao);
        glViewport(0, 0, window->width, window->height);
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
