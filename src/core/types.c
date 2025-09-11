#include "types.h"

#include <stdio.h>

vec2 *setVec2(vec2 *pos, float x, float y) {
    pos->x = x;
    pos->y = y;
    return pos;
}

vec2 *cpyVec2(vec2 *dst, vec2 *src) {
    dst->x = src->x;
    dst->y = src->y;
    return dst;
}

#define oppVec2(opp) \
    dst->x = a->x opp b->x; \
    dst->y = a->y opp b->y; \
    return dst;

#define oppVec2I(opp) \
    dst->x opp ## = src->x; \
    dst->y opp ## = src->y; \
    return dst;

#define oppVec2V(opp) \
    dst->x = src->x opp x; \
    dst->y = src->y opp y; \
    return dst;

#define oppVec2VI(opp) \
    dst->x opp ## = x; \
    dst->y opp ## = y; \
    return dst;

#define oppVec2S(opp) \
    dst->x = src->x opp val; \
    dst->y = src->y opp val; \
    return dst;

#define oppVec2SI(opp) \
    dst->x opp ## = val; \
    dst->y opp ## = val; \
    return dst;

vec2 *addVec2(vec2 *dst, vec2 *a, vec2 *b) { oppVec2(+) }
vec2 *addVec2I(vec2 *dst, vec2 *src) { oppVec2I(+) }
vec2 *addVec2V(vec2 *dst, vec2 *src, float x, float y) { oppVec2V(+) }
vec2 *addVec2VI(vec2 *dst, float x, float y) { oppVec2VI(+) }
vec2 *addVec2S(vec2 *dst, vec2 *src, float val) { oppVec2S(+) }
vec2 *addVec2SI(vec2 *dst, float val) { oppVec2SI(+) }

vec2 *subVec2(vec2 *dst, vec2 *a, vec2 *b) { oppVec2(-) }
vec2 *subVec2I(vec2 *dst, vec2 *src) { oppVec2I(-) }
vec2 *subVec2V(vec2 *dst, vec2 *src, float x, float y) { oppVec2V(-) }
vec2 *subVec2VI(vec2 *dst, float x, float y) { oppVec2VI(-) }
vec2 *subVec2S(vec2 *dst, vec2 *src, float val) { oppVec2S(-) }
vec2 *subVec2SI(vec2 *dst, float val) { oppVec2SI(-) }

vec2 *divVec2(vec2 *dst, vec2 *a, vec2 *b) { oppVec2(/) }
vec2 *divVec2I(vec2 *dst, vec2 *src) { oppVec2I(/) }
vec2 *divVec2V(vec2 *dst, vec2 *src, float x, float y) { oppVec2V(/) }
vec2 *divVec2VI(vec2 *dst, float x, float y) { oppVec2VI(/) }
vec2 *divVec2S(vec2 *dst, vec2 *src, float val) { oppVec2S(/) }
vec2 *divVec2SI(vec2 *dst, float val) { oppVec2SI(/) }

vec2 *mltVec2(vec2 *dst, vec2 *a, vec2 *b) { oppVec2(*) }
vec2 *mltVec2I(vec2 *dst, vec2 *src) { oppVec2I(*) }
vec2 *mltVec2V(vec2 *dst, vec2 *src, float x, float y) { oppVec2V(*) }
vec2 *mltVec2VI(vec2 *dst, float x, float y) { oppVec2VI(*) }
vec2 *mltVec2S(vec2 *dst, vec2 *src, float val) { oppVec2S(*) }
vec2 *mltVec2SI(vec2 *dst, float val) { oppVec2SI(*) }

void printVec2(vec2 *vec, const char *end) {
    printf("(%g, %g)%s", vec->x, vec->y, end);
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

void setColRGB(colRGB *col, uint8_t r, uint8_t g, uint8_t b) {
    col->r = r;
    col->g = g;
    col->b = b;
}

void cpyColRGB(colRGB *dst, colRGB *src) {
    dst->r = src->r;
    dst->g = src->g;
    dst->b = src->b;
}

void setColRGBA(colRGBA *col, uint8_t r, uint8_t g, uint8_t b, uint8_t a) {
    col->r = r;
    col->g = g;
    col->b = b;
    col->a = a;
}

void cpyColRGBA(colRGBA *dst, colRGBA *src) {
    dst->r = src->r;
    dst->g = src->g;
    dst->b = src->b;
    dst->a = src->a;
}
