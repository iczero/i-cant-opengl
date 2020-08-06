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

CubeGeometry::CubeGeometry(float side_length): Geometry(), side_length(side_length) {
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

    unbind();
    unbind_buffers();
}

void CubeGeometry::draw() {
    bind();
    glDrawElements(GL_TRIANGLES, index_count, GL_UNSIGNED_INT, 0);
}

glm::vec3 polar_to_cartesian(float radius, float azimuth, float elevation) {
    return glm::vec3(
        radius * glm::cos(glm::radians(elevation)) * glm::cos(glm::radians(azimuth)),
        radius * glm::sin(glm::radians(elevation)),
        radius * glm::cos(glm::radians(elevation)) * glm::sin(glm::radians(azimuth))
    );
}

IcosphereGeometry::IcosphereGeometry(unsigned int levels):
    Geometry(), levels(levels)
{
    construct();
    upload();
}

void IcosphereGeometry::generate_icosahedron() {
    // bottom vertex
    emit_vertex(polar_to_cartesian(1, 0, -90));
    // vertices on lower plane
    emit_vertex(polar_to_cartesian(1, 0, -30));
    emit_vertex(polar_to_cartesian(1, 72, -30));
    emit_vertex(polar_to_cartesian(1, 144, -30));
    emit_vertex(polar_to_cartesian(1, 216, -30));
    emit_vertex(polar_to_cartesian(1, 288, -30));
    // vertices on upper plane
    emit_vertex(polar_to_cartesian(1, 36, 30));
    emit_vertex(polar_to_cartesian(1, 108, 30));
    emit_vertex(polar_to_cartesian(1, 180, 30));
    emit_vertex(polar_to_cartesian(1, 252, 30));
    emit_vertex(polar_to_cartesian(1, 324, 30));
    // top vertex
    emit_vertex(polar_to_cartesian(1, 0, 90));
}

void IcosphereGeometry::subdivide_all_faces() {
    subdivide_face(0, 1, 2);
    subdivide_face(0, 2, 3);
    subdivide_face(0, 3, 4);
    subdivide_face(0, 4, 5);
    subdivide_face(0, 5, 1);
    subdivide_face(1, 6, 2);
    subdivide_face(2, 7, 3);
    subdivide_face(3, 8, 4);
    subdivide_face(4, 9, 5);
    subdivide_face(5, 10, 1);
    subdivide_face(6, 7, 2);
    subdivide_face(7, 8, 3);
    subdivide_face(8, 9, 4);
    subdivide_face(9, 10, 5);
    subdivide_face(10, 6, 1);
    subdivide_face(11, 7, 6);
    subdivide_face(11, 8, 7);
    subdivide_face(11, 9, 8);
    subdivide_face(11, 10, 9);
    subdivide_face(11, 6, 10);
}

void IcosphereGeometry::emit_vertex(glm::vec3 vertex) {
    vertices.push_back(glm::normalize(vertex));
}

void IcosphereGeometry::emit_face(unsigned int i, unsigned int j, unsigned int k) {
    indices.push_back(i);
    indices.push_back(j);
    indices.push_back(k);
}

void IcosphereGeometry::subdivide_face(unsigned int i, unsigned int j, unsigned int k) {
    // oh boy is this going to need a lot of explanation
    // vertices of sub-triangles belonging to this face
    std::vector<unsigned int> face_vertexes;
    // vertical coordinates within triangle (along height)
    for (auto y = 0U; y <= levels; y++) {
        // horizontal coordinates within triangle (along base)
        for (auto x = 0U; x <= y; x++) {
            if (y == 0 && x == 0) {
                // this is the "top" vertex of the triangle
                face_vertexes.push_back(i);
            } else if (y == levels && x == 0) {
                // bottom left vertex of triangle
                face_vertexes.push_back(j);
            } else if (y == levels && x == levels) {
                // bottom right vertex of triangle
                face_vertexes.push_back(k);
            } else {
                // in the middle of the triangle
                // barycentric coordinates for position of new vertex
                float fk = (float) x / y;
                float fj = 1.f - fk;
                float fi = (float) y / levels;
                fj *= fi;
                fk *= fi;
                fi = 1.f - fi;
                // barycentric -> cartesian, normalize to sphere, and add vertex
                emit_vertex(glm::vec3(
                    fi * vertices[i].x + fj * vertices[j].x + fk * vertices[k].x,
                    fi * vertices[i].y + fj * vertices[j].y + fk * vertices[k].y,
                    fi * vertices[i].z + fj * vertices[j].z + fk * vertices[k].z
                ));
                // last pushed element
                face_vertexes.push_back(vertices.size() - 1);
            }
        }
    }
    // generate all faces
    for (auto y = 0U; y < levels; y++) {
        // equivalent to 1 + 2 + 3 + ... + n
        // start of current row of triangle vertices
        int base = y * (y + 1) / 2;
        for (auto x = 0U; x <= y; x++) {
            // for "right side up" triangles
            emit_face(
                // vertex at current point (top vertex)
                face_vertexes[base + x],
                // vertex at next row (bottom left vertex)
                face_vertexes[base + y + x + 1],
                // vertex at next row plus 1 (bottom right vertex)
                face_vertexes[base + y + x + 2]
            );
        }
        if (y == 0) continue; // nothing to do here
        for (auto x = 0U; x <= y - 1; x++) {
            // the same, but for "upside-down" triangles
            emit_face(
                // top left
                face_vertexes[base + x],
                // top right
                face_vertexes[base + x + 1],
                // bottom
                face_vertexes[base + y + x + 2]
            );
        }
    }
}

void IcosphereGeometry::construct() {
    generate_icosahedron();
    subdivide_all_faces();
}

void IcosphereGeometry::upload() {
    bind();
    bind_buffers();
    // write vertex data to buffer
    std::vector<float> vertices_upload;
    vertices_upload.reserve(vertices.size() * 3);
    for (auto &vertex : vertices) {
        vertices_upload.push_back(vertex.x);
        vertices_upload.push_back(vertex.y);
        vertices_upload.push_back(vertex.z);
    }
    unsigned int buffer_size = vertices_upload.size() * sizeof(float);
    glBufferData(GL_ARRAY_BUFFER, buffer_size, &vertices_upload[0], GL_STATIC_DRAW);
    // write element data to buffer
    buffer_size = indices.size() * sizeof(unsigned int);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, buffer_size, &indices[0], GL_STATIC_DRAW);
    unbind();
    unbind_buffers();
}

void IcosphereGeometry::draw() {
    bind();
    glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, 0);
}
