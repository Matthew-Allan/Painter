#ifndef TYPES_H
#define TYPES_H

#include <stdint.h>

typedef struct vec2 {
    union {float x; float w;};
    union {float y; float h;};
} vec2;

typedef vec2 dim2;

typedef struct colRGB {
    uint8_t r;
    uint8_t g;
    uint8_t b;
} colRGB;

typedef struct colRGBA {
    uint8_t r;
    uint8_t g;
    uint8_t b;
    uint8_t a;
} colRGBA;

void printVec2(vec2 *vec, const char *end);

vec2 *setVec2(vec2 *pos, float x, float y);
vec2 *cpyVec2(vec2 *dst, vec2 *src);

vec2 *addVec2(vec2 *dst, vec2 *a, vec2 *b);
vec2 *addVec2I(vec2 *dst, vec2 *src);
vec2 *addVec2V(vec2 *dst, vec2 *src, float x, float y);
vec2 *addVec2VI(vec2 *dst, float x, float y);
vec2 *addVec2S(vec2 *dst, vec2 *src, float val);
vec2 *addVec2SI(vec2 *dst, float val);

vec2 *subVec2(vec2 *dst, vec2 *a, vec2 *b);
vec2 *subVec2I(vec2 *dst, vec2 *src);
vec2 *subVec2V(vec2 *dst, vec2 *src, float x, float y);
vec2 *subVec2VI(vec2 *dst, float x, float y);
vec2 *subVec2S(vec2 *dst, vec2 *src, float val);
vec2 *subVec2SI(vec2 *dst, float val);

vec2 *divVec2(vec2 *dst, vec2 *a, vec2 *b);
vec2 *divVec2I(vec2 *dst, vec2 *src);
vec2 *divVec2V(vec2 *dst, vec2 *src, float x, float y);
vec2 *divVec2VI(vec2 *dst, float x, float y);
vec2 *divVec2S(vec2 *dst, vec2 *src, float val);
vec2 *divVec2SI(vec2 *dst, float val);

vec2 *mltVec2(vec2 *dst, vec2 *a, vec2 *b);
vec2 *mltVec2I(vec2 *dst, vec2 *src);
vec2 *mltVec2V(vec2 *dst, vec2 *src, float x, float y);
vec2 *mltVec2VI(vec2 *dst, float x, float y);
vec2 *mltVec2S(vec2 *dst, vec2 *src, float val);
vec2 *mltVec2SI(vec2 *dst, float val);

#define setDim2(dim, w, h) setVec2(dim, w, h)
#define cpyDim2(dst, src) setVec2(dst, src)

void setColRGB(colRGB *col, uint8_t r, uint8_t g, uint8_t b);
void cpyColRGB(colRGB *dst, colRGB *src);
void setColRGBA(colRGBA *col, uint8_t r, uint8_t g, uint8_t b, uint8_t a);
void cpyColRGBA(colRGBA *dst, colRGBA *src);


#endif