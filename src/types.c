#include "types.h"

void setPixelPosi(pixelPosi *pos, int x, int y) {
    pos->x = x;
    pos->y = y;
}

void setPixelPos(pixelPos *pos, float x, float y) {
    pos->x = x;
    pos->y = y;
}

void setPixelDim(pixelDim *dim, int w, int h) {
    dim->w = w;
    dim->h = h;
}

void setColourRGB(colourRGB *col, uint8_t r, uint8_t g, uint8_t b) {
    col->r = r;
    col->g = g;
    col->b = b;
}

void setColourRGBA(colourRGBA *col, uint8_t r, uint8_t g, uint8_t b, uint8_t a) {
    col->r = r;
    col->g = g;
    col->b = b;
    col->a = a;
}

void cpyPixelPosi(pixelPosi *dst, pixelPosi *src) {
    dst->x = src->x;
    dst->y = src->y;
}

void cpyPixelPos(pixelPos *dst, pixelPos *src) {
    dst->x = src->x;
    dst->y = src->y;
}

void cpyPixelDim(pixelDim *dst, pixelDim *src) {
    dst->w = src->w;
    dst->h = src->h;
}

void cpyColourRGB(colourRGB *dst, colourRGB *src) {
    dst->r = src->r;
    dst->g = src->g;
    dst->b = src->b;
}

void cpyColourRGBA(colourRGBA *dst, colourRGBA *src) {
    dst->r = src->r;
    dst->g = src->g;
    dst->b = src->b;
    dst->a = src->a;
}
