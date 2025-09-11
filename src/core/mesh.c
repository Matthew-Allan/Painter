#include "mesh.h"

#include <sys/types.h>

void genSquareMesh(float top, float btm, float lft, float rgt, meshData *mesh) {
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
    glGenVertexArrays(1, &mesh->vao);

    // Bind the vertex array object.
    glBindVertexArray(mesh->vao);

    // Generate buffer objects.
    glGenBuffers(1, &mesh->vbo);
    glGenBuffers(1, &mesh->ebo);

    // Bind the virtual and element buffer objects.
    glBindBuffer(GL_ARRAY_BUFFER, mesh->vbo);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mesh->ebo);

    // Send the vertex and index data to the buffer objects.
    glBufferData(GL_ARRAY_BUFFER, sizeof(verticies), verticies, GL_STATIC_DRAW);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

    // Tell the shader how to interpret the VBO.
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), 0);

    // Enable the attributes.
    glEnableVertexAttribArray(0);
}

void closeMesh(meshData *mesh) {
    glBindVertexArray(mesh->vao);
    glDeleteBuffers(1, &mesh->ebo);
    glDeleteBuffers(1, &mesh->vbo);
    glDeleteVertexArrays(1, &mesh->vao);
}