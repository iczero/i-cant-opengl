#pragma once

#include <string>
#include <vector>
#include "Octree.h"

using std::string;

class NBody {
public:
    NBody()
    {
        OctreeNode* node = new OctreeNode();
        Vec3* half = new Vec3(50, 50, 50);
        Vec3* origin = new Vec3(50, 50, 50);
        Octree* tree = new Octree(node, half, origin);
        m_bodies = tree;

    }
    struct Vec3FloatPair {
        Vec3 vec3;
        float num;
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
    static Vec3 calculate_force(OctreeNode*, OctreeNode&&);
    static Vec3 calculate_force(OctreeNode*, OctreeNode*);
    static Vec3 calculate_center_of_mass(Octree*);
    static void update_position(OctreeNode*);
    Octree* simulate_frame();
    void print();








private:
    Octree* m_bodies;
};
