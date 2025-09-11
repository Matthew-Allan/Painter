#ifndef MESH_H
#define MESH_H

#include <glad/glad.h>

typedef struct meshData {
    GLuint vao;
    GLuint vbo;
    GLuint ebo;
} meshData;

void genSquareMesh(float top, float btm, float lft, float rgt, meshData *mesh);
void closeMesh(meshData *mesh);

#endif