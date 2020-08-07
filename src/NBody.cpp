#include <iostream>
#include <fstream>
#include <string>
#include <stdlib.h>
#include "NBody.h"
#include "Octree.h"

void NBody::read_bodies_from_csv(const char* file_name)
{
    //std::cout << "Do I even start?\n";
    std::ifstream csv_file(file_name);
    string line;
    getline(csv_file, line);
    int i = 0;
    for (; getline(csv_file, line);) {
        string x_coordinate_string;
        for (; line[i] != ','; i++)
            x_coordinate_string.push_back(line[i]);
        double x_coordinate = stod(x_coordinate_string);
        x_coordinate_string.clear();
        i++;

        string y_coordinate_string;
        for (; line[i] != ','; i++)
            y_coordinate_string.push_back(line[i]);
        double y_coordinate = stod(y_coordinate_string);
        y_coordinate_string.clear();
        i++;

        string z_coordinate_string;
        for (; line[i] != ','; i++)
            z_coordinate_string.push_back(line[i]);
        double z_coordinate = stod(z_coordinate_string);
        z_coordinate_string.clear();
        i++;


        string x_velocity_string;
        for (; line[i] != ','; i++)
            x_velocity_string.push_back(line[i]);
        double x_velocity = stod(x_velocity_string);
        x_velocity_string.clear();
        i++;

        string y_velocity_string;
        for (; line[i] != ','; i++)
            y_velocity_string.push_back(line[i]);
        double y_velocity = stod(y_velocity_string);
        y_velocity_string.clear();
        i++;

        string z_velocity_string;
        for (; line[i] != ','; i++)
            z_velocity_string.push_back(line[i]);
        double z_velocity = stod(z_velocity_string);
        z_velocity_string.clear();
        i++;


        string x_force_string;
        for (; line[i] != ','; i++)
            x_force_string.push_back(line[i]);
        double x_force = stod(x_force_string);
        x_force_string.clear();
        i++;

        string y_force_string;
        for (; line[i] != ','; i++)
            y_force_string.push_back(line[i]);
        double y_force = stod(y_force_string);
        y_force_string.clear();
        i++;

        string z_force_string;
        for (; line[i] != ','; i++)
            z_force_string.push_back(line[i]);
        double z_force = stod(z_force_string);
        z_force_string.clear();
        i++;

        string mass_string;
        for (; line[i] != ','; i++)
            mass_string.push_back(line[i]);
        double mass = stod(mass_string);
        mass_string.clear();
        i++;

        string radius_string;
        for (; i < line.size(); i++)
            radius_string.push_back(line[i]);
        double radius = stod(radius_string);
        radius_string.clear();
        
        Vec3 position_vec = Vec3 { x_coordinate, y_coordinate, z_coordinate };
        Vec3 velocity_vec = Vec3 { x_velocity, y_velocity, z_velocity };
        Vec3 force_vec = Vec3 { x_force, y_force, z_force };

        m_bodies->insert(new OctreeNode { position_vec, velocity_vec, force_vec, mass, radius});

        i = 0;
    }
    /*
    for (int i=0; i < m_bodies.size(); i++) {
        std::cout << "x-coord: " << m_bodies[i].x_coordinate
                  << " y-coord: " << m_bodies[i].y_coordinate
                  << " z-coord: " << m_bodies[i].z_coordinate
                  << " mass: " << m_bodies[i].mass << '\n';
    }
    */
}

void NBody::randomly_generate_bodies(int count)
{
    void *nothing = malloc(1);
    srand((unsigned long long)nothing); // Lol
    free(nothing);
    for (int i=0; i < count; i++) { // was 10000
        Vec3 position_vec = Vec3 {
            (double)(rand() % 10000) / (double)10 - 500,
            (double)(rand() % 10000) / (double)10 - 500,
            (double)(rand() % 10000) / (double)10 - 500 };
        /*
        Vec3 velocity_vec = Vec3 {
            (double)(rand() % 1000) / (double)10 - 50,
            (double)(rand() % 1000) / (double)10 - 50,
            (double)(rand() % 1000) / (double)10 - 50 };
        */
        Vec3 velocity_vec = Vec3(0, 0, 0);
        Vec3 force_vec = Vec3(0, 0, 0);

        /* in case new templates need to be created
        std::cout << position_vec.x
                  << ',' << position_vec.y
                  << ',' << position_vec.z
                  << ',' << velocity_vec.x
                  << ',' << velocity_vec.y
                  << ',' << velocity_vec.z
                  << ',' << force_vec.x
                  << ',' << force_vec.y
                  << ',' << force_vec.z
                  << ',' << (double)(rand() % 1000) / (double)10 - 50
                  << ',' << (double)(rand() % 1000) / (double)10 - 50 << '\n';
        */

        // m_bodies->insert(new OctreeNode { position_vec, velocity_vec, force_vec, (double)(rand() % 1000) / (double)10 - 50, (double)(rand() % 1000) / (double)10 - 50 });
        m_bodies->insert(new OctreeNode { position_vec, velocity_vec, force_vec, (double)(rand() % 1000) / (double)10, (double)(rand() % 100000) * 1e9 });
    }
}

void NBody::print()
{
    m_bodies->printOctree(17);
}

struct TwoSums {
    Vec3 sum1;
    double sum2;
};
NBody::Vec3FloatPair calculate_center_of_mass_helper(Octree* tree)
{
    NBody::Vec3FloatPair sums = { { 0, 0, 0 }, 1 };
    for (int i=0; i < 8; i++) {
        if (!tree->getChildren()[i])
            continue;
        if (!tree->getChildren()[i]->isLeafNode()) {
            sums += calculate_center_of_mass_helper(tree->getChildren()[i]); // This really needs to take into account both sums
            continue;
        }
        if (!tree->getChildren()[i]->getData())
            continue;
        sums.vec3 += tree->getChildren()[i]->getData()->position * tree->getChildren()[i]->getData()->mass;
        sums.num += tree->getChildren()[i]->getData()->mass;
    }
    return sums;
}


Vec3 NBody::calculate_center_of_mass(Octree* tree) // very segfaulty
{
    NBody::Vec3FloatPair result = calculate_center_of_mass_helper(tree);
    return result.vec3 / result.num;
}

double mass_from_body_cluster(Octree* tree) // Probably segfaulty
{
    double sum = 0;
    for (int i=0; i < 8; i++) {
        if (!tree->getChildren()[i])
            continue;
        if (!tree->getChildren()[i]->isLeafNode()) {
            sum += mass_from_body_cluster(tree->getChildren()[i]); // Does this work?
            continue;
        }
        if (tree->getChildren()[i]->getData())
            sum += tree->getChildren()[i]->getData()->mass;
    }
    return sum;
}

Vec3 NBody::calculate_force(OctreeNode* node_a, OctreeNode&& node_b)
{
    if (!node_a)
        std::cout << "Sad face from calculate_force*&& :(\n";

    Vec3 diff = node_b.position - node_a->position;
    return diff.normalize() * (6.67408e-11 * node_a->mass * node_b.mass) / pow(diff.magnitude(), 2);
    /*
    return Vec3 { (6.674e-11 * node_a->mass * node_b.mass) / sqrt((node_a->position.x - node_b.position.x) - (node_a->radius + node_b.radius)),
                  (6.674e-11 * node_a->mass * node_b.mass) / sqrt((node_a->position.y - node_b.position.y) - (node_a->radius + node_b.radius)),
                  (6.674e-11 * node_a->mass * node_b.mass) / sqrt((node_a->position.z - node_b.position.z) - (node_a->radius + node_b.radius)) };
    */
}
Vec3 NBody::calculate_force(OctreeNode* node_a, OctreeNode* node_b)
{
    if (!node_a || !node_b)
        std::cout << "Sad face from calculate_force** :(\n";
    Vec3 diff = node_b->position - node_a->position;
    return diff.normalize() * (6.67408e-11 * node_a->mass * node_b->mass) / pow(diff.magnitude(), 2);
    /*
    return Vec3 { (6.674e-11 * node_a->mass * node_b->mass) / sqrt((node_a->position.x - node_b->position.x) - (node_a->radius + node_b->radius)),
                  (6.674e-11 * node_a->mass * node_b->mass) / sqrt((node_a->position.y - node_b->position.y) - (node_a->radius + node_b->radius)),
                  (6.674e-11 * node_a->mass * node_b->mass) / sqrt((node_a->position.z - node_b->position.z) - (node_a->radius + node_b->radius)) };
    */
}
void NBody::calculate_force_and_acceleration(Octree* tree, OctreeNode* node)
{
    if (!node || tree->getData() == node || !tree || (!tree->getData() && tree->isLeafNode())) {
        return;
    }
    if (tree->isLeafNode()) {
        // std::cout << "calculated force: " << calculate_force(tree->getData(), node).x << std::endl;
        node->force -= calculate_force(tree->getData(), node);
        return;
    }
    int octant = (m_bodies->getOctant(&node->position) + 8) / 8;
    double octant_width = 1000 / octant; // 1000 is the hardcoded width of the head octant. Change this to be setable by the user?
    Vec3 center_of_mass = calculate_center_of_mass(tree);
    Vec3 distance = center_of_mass - node->position;

    if (octant_width / distance.magnitude() < 0.5) { // then can be treated as a single body
        node->force -= calculate_force(node, OctreeNode { center_of_mass, node->velocity, node->force, 0, mass_from_body_cluster(tree) });
        m_bodies->remove(node);
        m_bodies->insert(node);
        return;
    }
    else 
        for (int i=0; i < 8; i++)
            calculate_force_and_acceleration(tree->getChildren()[i], node);
}

void NBody::update_position(OctreeNode* node, double time)
{
    node->acceleration = Vec3(
        node->force.x / node->mass,
        node->force.y / node->mass,
        node->force.z / node->mass
    );
    node->velocity += node->acceleration * time;
    node->position += node->velocity * time;
    // std::cout << "force: " << node->force.x << " " << node->force.y << " " << node->force.z << std::endl;
    node->force.x = 0;
    node->force.y = 0;
    node->force.z = 0;
}

Octree* NBody::simulate_frame() // Should return the whole octree so it can be displayed by OpenGL
{
    for (auto *node : m_bodies->allNodes)
        calculate_force_and_acceleration(m_bodies, node); // only if node is a body. Figure that out.

    for (auto *node : m_bodies->allNodes)
        update_position(node, 1.0);
    return m_bodies;
}

/*
int main(int argc, char** argv)
{
    NBody nbody;
    if (argc == 1)
        nbody.randomly_generate_bodies();
    else
        for (int i = 1; i < argc; i++)
            nbody.read_bodies_from_csv(argv[i]);
    std::cout << "Finished sucking up data\n";
    //nbody.print();
    for (int i=0; i < 100; i++) {
        nbody.simulate_frame();
    }
}
*/
