#pragma once
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <vector>
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
    float side_length;

    // note: this is a bad idea and should not be done
    CubeGeometry(float side_length);
    void draw();
};

class IcosphereGeometry : public Geometry {
public:
    // how many times to subdivide icosahedron faces
    unsigned int levels;
    std::vector<glm::vec3> vertices;
    std::vector<unsigned int> indices;

    IcosphereGeometry(unsigned int levels);
    // construct sphere
    void construct();
    // upload sphere to gpu
    void upload();
    // generates icosahedron and adds to vertices
    void generate_icosahedron();
    // runs subdivide_face on all icosahedron faces
    void subdivide_all_faces();
    // create and normalize vertex to sphere surface
    void emit_vertex(glm::vec3 vertex);
    // add face to index buffer
    void emit_face(unsigned int i, unsigned int j, unsigned int k);
    // subdivide face at vertex indices
    void subdivide_face(unsigned int i, unsigned int j, unsigned int k);
    // draw the thing
    void draw();
};
