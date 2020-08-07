#pragma once

#include <string>
#include <vector>
#include <glm/glm.hpp>
#include "octree.hpp"

using std::string;

class NBody {
public:
    Octree* m_bodies;

    NBody();
    struct Vec3FloatPair {
        glm::dvec3 vec3;
        double num;
        Vec3FloatPair& operator+=(Vec3FloatPair pair)
        {
            vec3 += pair.vec3;
            num += pair.num;
            return *this;
        }
    };
    void read_bodies_from_csv(const char* file_name);
    void randomly_generate_bodies(int count);
    void calculate_force_and_acceleration(Octree*, OctreeNode*);
    static glm::dvec3 calculate_force(OctreeNode*, OctreeNode&&);
    static glm::dvec3 calculate_force(OctreeNode*, OctreeNode*);
    static glm::dvec3 calculate_center_of_mass(Octree*);
    void update_position(OctreeNode &, double time = 1.0);
    Octree* simulate_frame();
    void print();
};
