#include "types.h"

void setIVec2(ivec2 *pos, int x, int y) {
    pos->x = x;
    pos->y = y;
}

void setVec2(vec2 *pos, float x, float y) {
    pos->x = x;
    pos->y = y;
}

void setColRGB(colRGB *col, uint8_t r, uint8_t g, uint8_t b) {
    col->r = r;
    col->g = g;
    col->b = b;
}

void setColRGBA(colRGBA *col, uint8_t r, uint8_t g, uint8_t b, uint8_t a) {
    col->r = r;
    col->g = g;
    col->b = b;
    col->a = a;
}

void cpyIVec2(ivec2 *dst, ivec2 *src) {
    dst->x = src->x;
    dst->y = src->y;
}

void cpyVec2(vec2 *dst, vec2 *src) {
    dst->x = src->x;
    dst->y = src->y;
}

void cpyColRGB(colRGB *dst, colRGB *src) {
    dst->r = src->r;
    dst->g = src->g;
    dst->b = src->b;
}

void cpyColRGBA(colRGBA *dst, colRGBA *src) {
    dst->r = src->r;
    dst->g = src->g;
    dst->b = src->b;
    dst->a = src->a;
}
