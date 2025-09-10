#ifndef TYPES_H
#define TYPES_H

#include <stdint.h>

typedef struct vec2 {
    union {float x; float w;};
    union {float y; float h;};
} vec2;

typedef vec2 dim2;

void setVec2(vec2 *pos, float x, float y);
void cpyVec2(vec2 *dst, vec2 *src);

#define setDim2(dim, w, h) setVec2(dim, w, h);
#define cpyDim2(dst, src) setVec2(dst, src);

typedef struct ivec2 {
    int x;
    int y;
} ivec2;

void setIVec2(ivec2 *pos, int x, int y);
void cpyIVec2(ivec2 *dst, ivec2 *src);

typedef struct colRGB {
    uint8_t r;
    uint8_t g;
    uint8_t b;
} colRGB;

void setColRGB(colRGB *col, uint8_t r, uint8_t g, uint8_t b);
void cpyColRGB(colRGB *dst, colRGB *src);

typedef struct colRGBA {
    uint8_t r;
    uint8_t g;
    uint8_t b;
    uint8_t a;
} colRGBA;

void setColRGBA(colRGBA *col, uint8_t r, uint8_t g, uint8_t b, uint8_t a);
void cpyColRGBA(colRGBA *dst, colRGBA *src);

#endif