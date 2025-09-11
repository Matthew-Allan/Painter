#ifndef CANVAS_H
#define CANVAS_H

#include "types.h"
#include "window.h"

typedef struct canvasShader {
    GLuint prog;
    GLuint mouse;
    GLuint prev_mouse;
    GLuint colour;
    GLuint size;
} canvasShader;

typedef struct Canvas {
    canvasShader shader;
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
    int pen_size;
    colRGBA pen_col;
    float zoom_scale;
    float zoom;
    uint zoom_updt : 1;
    uint off_updt : 1;
    uint col_updt : 1;
    uint size_updt : 1;
} Canvas;

int initCanvas(Canvas *canvas, dispWindow *window, int width, int height);

void updateCanvas(Canvas *canvas);

void calculatePenPos(Canvas *canvas, dispWindow *window);
void zoomCentered(Canvas *canvas, dispWindow *window, vec2 *canv_center, vec2 *wind_center, float zoom);
void rstBaseOff(Canvas *canvas);
void updateOff(Canvas *canvas);

#endif