#pragma once
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include "util.hpp"

// reperesents a drawable geometry object
class Geometry {
public:
    // opengl objects
    unsigned int vao;
    unsigned int vbo;
    unsigned int ebo;

    // number of elements in index array
    unsigned int index_count;

    Geometry();
    ~Geometry();

    // bind vertex array
    void bind();
    void unbind();

    // bind buffers
    void bind_buffers();
    void unbind_buffers();
};

class CubeGeometry : public Geometry {
public:
    CubeGeometry(float side_length);
    void draw();
};
