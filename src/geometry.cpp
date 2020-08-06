#include "geometry.hpp"
#include <glad/glad.h>

Geometry::Geometry() {
    // generate own vertex array object
    glGenVertexArrays(1, &vao);
    // generate vertex and element buffer objects
    glGenBuffers(1, &vbo);
    glGenBuffers(1, &ebo);

    bind();
    bind_buffers();
    // only one shader is used now, so set vertex format here
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void *) 0);
    glEnableVertexAttribArray(0);
    unbind();
    unbind_buffers();
}

Geometry::~Geometry() {
    glDeleteVertexArrays(1, &vao);
    glDeleteBuffers(1, &vbo);
    glDeleteBuffers(1, &ebo);
}

void Geometry::bind() {
    glBindVertexArray(vao);
}

void Geometry::unbind() {
    glBindVertexArray(0);
}

void Geometry::bind_buffers() {
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
}

void Geometry::unbind_buffers() {
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}

CubeGeometry::CubeGeometry(float side_length): Geometry() {
    bind();
    bind_buffers();
    // a cube
    float s = side_length;
    const float vertices[] = {
        // front 4 vertices, reading order
        -s, s, s,
        s, s, s,
        -s, -s, s,
        s, -s, s,
        -s, s, -s,
        s, s, -s,
        -s, -s, -s,
        s, -s, -s
    };

    const unsigned int indices[] = {
        // in no particular order
        0, 1, 2,   1, 2, 3,
        0, 4, 5,   0, 5, 1,
        1, 5, 7,   1, 7, 3,
        2, 6, 7,   2, 7, 3,
        0, 4, 6,   0, 6, 2,
        4, 5, 6,   5, 6, 7
    };

    index_count = array_size(indices);

    // write vertex data to buffer
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
    // write element data to buffer
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);
}

void CubeGeometry::draw() {
    bind();
    glDrawElements(GL_TRIANGLES, index_count, GL_UNSIGNED_INT, 0);
}
