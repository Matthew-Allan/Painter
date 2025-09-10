#ifndef TYPES_H
#define TYPES_H

#include <stdint.h>

typedef struct pixelPosi {
    int x;
    int y;
} pixelPosi;

void setPixelPosi(pixelPosi *pos, int x, int y);
void cpyPixelPosi(pixelPosi *dst, pixelPosi *src);

typedef struct pixelPos {
    float x;
    float y;
} pixelPos;

void setPixelPos(pixelPos *pos, float x, float y);
void cpyPixelPos(pixelPos *dst, pixelPos *src);

typedef struct pixelDim {
    int w;
    int h;
} pixelDim;

void setPixelDim(pixelDim *dim, int w, int h);
void cpyPixelDim(pixelDim *dst, pixelDim *src);

typedef struct colourRGB {
    uint8_t r;
    uint8_t g;
    uint8_t b;
} colourRGB;

void setColourRGB(colourRGB *col, uint8_t r, uint8_t g, uint8_t b);
void cpyColourRGB(colourRGB *dst, colourRGB *src);

typedef struct colourRGBA {
    uint8_t r;
    uint8_t g;
    uint8_t b;
    uint8_t a;
} colourRGBA;

void setColourRGBA(colourRGBA *col, uint8_t r, uint8_t g, uint8_t b, uint8_t a);
void cpyColourRGBA(colourRGBA *dst, colourRGBA *src);

#endif