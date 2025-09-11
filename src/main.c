#include <stdlib.h>

#include <glad/glad.h>
#include "window.h"
#include "shader.h"

#include <math.h>

#define CANV_WIDTH 256
#define CANV_HEIGHT 128

#define TEX(tex) (GL_TEXTURE0 + tex)

#define CANVAS_TEX 0

typedef struct screenShader {
    GLuint prog;
    GLuint dim;
    GLuint off;
    GLuint zoom;
} screenShader;

typedef struct canvasShader {
    GLuint prog;
    GLuint mouse;
    GLuint prev_mouse;
    GLuint colour;
} canvasShader;

typedef struct Canvas {
    GLuint draw_vao;
    GLuint draw_vbo;
    GLuint FBO;
    GLuint tex;
    dim2 size;
    vec2 base_off;
    vec2 new_off;
    vec2 off;
    vec2 pen;
    vec2 prev_pen;
    colRGBA pen_col;
    float zoom_scale;
    float zoom;
    uint zoom_updt : 1;
    uint off_updt : 1;
    uint col_updt : 1;
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

void createCanvasVAO(Canvas *canvas) {
    genSquareVAO(canvas->size.h, 0, 0, canvas->size.w, &canvas->draw_vao, &canvas->draw_vbo);
}

void createCanvasTex(Canvas *canvas) {
    glActiveTexture(TEX(CANVAS_TEX));
    glGenTextures(1, &canvas->tex);
    glBindTexture(GL_TEXTURE_2D, canvas->tex);

    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, canvas->size.w, canvas->size.h, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);

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

    glClearColor(1, 1, 1, 1);
    glClear(GL_COLOR_BUFFER_BIT);

    return 0;
}

void uniformVec2(GLuint loc, vec2 *vec) {
    glUniform2f(loc, vec->x, vec->y);
}

void uniformIVec2(GLuint loc, vec2 *vec) {
    glUniform2i(loc, vec->x, vec->y);
}

void uniformColRGBA(GLuint loc, colRGBA *col) {
    glUniform4f(loc, col->r / 255.f, col->g / 255.f, col->b / 255.f, col->a / 255.f);
}

void mouseToPixPos(vec2 *pos, vec2 *scale, int x, int y) {
    mltVec2V(pos, scale, x, y);
}

void calculatePenPos(Canvas *canvas, dispWindow *window) {
    subVec2I(divVec2S(&canvas->pen, &window->mouse, canvas->zoom), &canvas->off);
}

void updateOff(Canvas *canvas) {
    addVec2(&canvas->off, &canvas->base_off, &canvas->new_off);
    canvas->off_updt = 1;
}

void rstBaseOff(Canvas *canvas) {
    setVec2(&canvas->new_off, 0, 0);
    cpyVec2(&canvas->base_off, &canvas->off);
}

void zoomCentered(Canvas *canvas, dispWindow *window, vec2 *canv_center, vec2 *wind_center, float zoom) {
    canvas->zoom_scale += zoom;
    canvas->zoom = canvas->zoom_scale * canvas->zoom_scale;
    canvas->zoom_updt = 1;
    cpyVec2(&window->rmb_down_pos, &window->mouse);
    rstBaseOff(canvas);
    subVec2I(divVec2S(&canvas->base_off, wind_center, canvas->zoom), canv_center);
    updateOff(canvas);
}

void updateCanvas(Canvas *canvas, canvasShader *shader) {
    glUseProgram(shader->prog);

    if(canvas->col_updt) {
        canvas->col_updt = 0;
        uniformColRGBA(shader->colour, &canvas->pen_col);
    }

    uniformIVec2(shader->mouse, &canvas->pen);
    uniformIVec2(shader->prev_mouse, &canvas->prev_pen);
    cpyVec2(&canvas->prev_pen, &canvas->pen);

    glBindFramebuffer(GL_FRAMEBUFFER, canvas->FBO);
    glBindVertexArray(canvas->draw_vao);
    glViewport(0, 0,canvas->size.w, canvas->size.h);
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
}

void updateScreen(Canvas *canvas, dispWindow *window, screenShader *shader, GLuint screen_vao) {
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
    glBindVertexArray(screen_vao);
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
        case SDL_MOUSEWHEEL:
            zoomCentered(canvas, window, &canvas->pen, &window->mouse, event.wheel.preciseY * 0.1);
            break;
        case SDL_MOUSEMOTION:
            mouseToPixPos(&window->mouse, &window->pix_scale, event.motion.x, event.motion.y);
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
                mouseToPixPos(&window->rmb_down_pos, &window->pix_scale, event.button.x, event.button.y);
            }
            break;
        default:
            break;
        }
    }
}

int runApp(dispWindow *window) {
    canvasShader canvas_shader;
    if(buildShader(&canvas_shader.prog, "shaders/canvas/vert.glsl", "shaders/canvas/frag.glsl") == -1) {
        return -1;
    }

    glUseProgram(canvas_shader.prog);
    glUniform1i(glGetUniformLocation(canvas_shader.prog, "canvas_tex"), CANVAS_TEX);

    canvas_shader.mouse = glGetUniformLocation(canvas_shader.prog, "mouse_pos");
    canvas_shader.prev_mouse = glGetUniformLocation(canvas_shader.prog, "prev_mouse_pos");
    canvas_shader.colour = glGetUniformLocation(canvas_shader.prog, "colour");
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
    setDim2(&canvas.size, CANV_WIDTH, CANV_HEIGHT);
    setColRGBA(&canvas.pen_col, 255, 200, 200, 255);
    divVec2SI(subVec2(&canvas.off, &window->size, &canvas.size), 2);
    rstBaseOff(&canvas);
    canvas.zoom_scale = 1;
    canvas.zoom = 1;
    canvas.zoom_updt = 1;
    canvas.off_updt = 1;
    canvas.col_updt = 1;
    createCanvasVAO(&canvas);
    if(createCanvasFBO(&canvas) == -1) {
        return -1;
    }

    GLuint screen_vao;
    genSquareVAO(canvas.size.h, 0, 0, canvas.size.w, &screen_vao, NULL);

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
            updateCanvas(&canvas, &canvas_shader);
        }

        updateScreen(&canvas, window, &screen_shader, screen_vao);

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
