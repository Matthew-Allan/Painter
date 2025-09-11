#include "canvas.h"

#include "tex.h"
#include "shader.h"

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

void updateCanvas(Canvas *canvas) {
    glUseProgram(canvas->shader.prog);

    if(canvas->col_updt) {
        canvas->col_updt = 0;
        uniformColRGBA(canvas->shader.colour, &canvas->pen_col);
    }
    if(canvas->size_updt) {
        canvas->size_updt = 0;
        glUniform1i(canvas->shader.size, canvas->pen_size);
    }

    uniformIVec2(canvas->shader.mouse, &canvas->pen);
    uniformIVec2(canvas->shader.prev_mouse, &canvas->prev_pen);
    cpyVec2(&canvas->prev_pen, &canvas->pen);

    glBindFramebuffer(GL_FRAMEBUFFER, canvas->FBO);
    glBindVertexArray(canvas->draw_vao);
    glViewport(0, 0,canvas->size.w, canvas->size.h);
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
}

int initCanvas(Canvas *canvas, dispWindow *window, int width, int height) {
    setDim2(&canvas->size, width, height);
    setColRGBA(&canvas->pen_col, 0, 0, 0, 255);
    divVec2SI(subVec2(&canvas->off, &window->size, &canvas->size), 2);
    rstBaseOff(canvas);
    canvas->zoom_scale = 1;
    canvas->zoom = 1;
    canvas->pen_size = 4;
    canvas->zoom_updt = 1;
    canvas->size_updt = 1;
    canvas->off_updt = 1;
    canvas->col_updt = 1;
    createCanvasVAO(canvas);
    if(createCanvasFBO(canvas) == -1) {
        return -1;
    }

    if(buildShader(&canvas->shader.prog, "shaders/canvas/vert.glsl", "shaders/canvas/frag.glsl") == -1) {
        return -1;
    }

    glUseProgram(canvas->shader.prog);
    glUniform1i(glGetUniformLocation(canvas->shader.prog, "canvas_tex"), CANVAS_TEX);

    canvas->shader.mouse = glGetUniformLocation(canvas->shader.prog, "mouse_pos");
    canvas->shader.prev_mouse = glGetUniformLocation(canvas->shader.prog, "prev_mouse_pos");
    canvas->shader.colour = glGetUniformLocation(canvas->shader.prog, "colour");
    canvas->shader.size = glGetUniformLocation(canvas->shader.prog, "size");
    return 0;
}