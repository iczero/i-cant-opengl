#include <iostream>
#include <fstream>
#include <string>
#include <stdlib.h>
#include "NBody.hpp"
#include "octree.hpp"

void NBody::read_bodies_from_csv(const char* file_name)
{
    std::ifstream csv_file(file_name);
    if (!csv_file) throw std::runtime_error("failed to load csv file");
    string line;
    getline(csv_file, line);
    int i = 0;
    do {
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


        string mass_string;
        for (; line[i] != ','; i++)
            mass_string.push_back(line[i]);
        // multiply by 1e10 as otherwise they will be to small to be rendered
        double mass = stod(mass_string) * 1e10;
        mass_string.clear();
        i++;
        
        glm::dvec3 position_vec = glm::dvec3 { x_coordinate, y_coordinate, z_coordinate };
        glm::dvec3 velocity_vec = glm::dvec3 { x_velocity, y_velocity, z_velocity };
        glm::dvec3 force_vec = glm::dvec3 { 0, 0, 0 };

        m_bodies->insert(new OctreeNode(position_vec, velocity_vec, force_vec, mass));

        i = 0;
    } while (getline(csv_file, line));
    /*
    for (int i=0; i < m_bodies.size(); i++) {
        std::cout << "x-coord: " << m_bodies[i].x_coordinate
                  << " y-coord: " << m_bodies[i].y_coordinate
                  << " z-coord: " << m_bodies[i].z_coordinate
                  << " mass: " << m_bodies[i].mass << '\n';
    }
    */
}

NBody::NBody() {
    glm::dvec3 half(10000, 10000, 10000);
    glm::dvec3 origin(0, 0, 0);
    Octree* tree = new Octree(nullptr, half, origin);
    m_bodies = tree;
}

void NBody::randomly_generate_bodies(int count)
{
    for (int i=0; i < count; i++) { // was 10000
        glm::dvec3 position_vec = glm::dvec3 {
            (double)(rand() % 100000) / (double)10 - 5000,
            (double)(rand() % 100000) / (double)10 - 5000,
            (double)(rand() % 100000) / (double)10 - 5000 };
        glm::dvec3 velocity_vec = glm::dvec3 {
            (double)(rand() % 100) / (double)10 - 5,
            (double)(rand() % 100) / (double)10 - 5,
            (double)(rand() % 100) / (double)10 - 5 };
        glm::dvec3 force_vec = glm::dvec3(0, 0, 0);

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

        m_bodies->insert(new OctreeNode(position_vec, velocity_vec, force_vec, (double)(rand() % 100000) * 1e9));
    }
}

void NBody::print()
{
    m_bodies->printOctree(17);
}

struct TwoSums {
    glm::dvec3 sum1;
    double sum2;
};

NBody::Vec3FloatPair& NBody::Vec3FloatPair::operator+=(Vec3FloatPair pair) {
    vec3 += pair.vec3;
    num += pair.num;
    return *this;
}
    
NBody::Vec3FloatPair calculate_center_of_mass_helper(Octree* tree)
{
    NBody::Vec3FloatPair sums = { { 0, 0, 0 }, 1 };
    for (int i = 0; i < 8; i++) {
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


glm::dvec3 NBody::calculate_center_of_mass(Octree* tree) // very segfaulty
{
    NBody::Vec3FloatPair result = calculate_center_of_mass_helper(tree);
    return result.vec3 / result.num;
}

double mass_from_body_cluster(Octree* tree) // Probably segfaulty
{
    double sum = 0;
    for (int i = 0; i < 8; i++) {
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

glm::dvec3 NBody::calculate_force(OctreeNode* node_a, OctreeNode&& node_b)
{
    if (!node_a)
        std::cout << "Sad face from calculate_force*&& :(\n";
    glm::dvec3 diff = node_b.position - node_a->position;
    return glm::normalize(diff) * (6.67408e-11 * node_a->mass * node_b.mass) / pow(glm::length(diff), 2);
}
glm::dvec3 NBody::calculate_force(OctreeNode* node_a, OctreeNode* node_b)
{
    if (!node_a || !node_b)
        std::cout << "Sad face from calculate_force** :(\n";
    glm::dvec3 diff = node_b->position - node_a->position;
    return glm::normalize(diff) * (6.67408e-11 * node_a->mass * node_b->mass) / pow(glm::length(diff), 2);
}
void NBody::calculate_force_and_acceleration(Octree* tree, OctreeNode* node)
{
    if (!node || !tree || tree->getData() == node || (!tree->getData() && tree->isLeafNode())) {
        return;
    }
    if (tree->isLeafNode()) {
        node->force -= calculate_force(tree->getData(), node);
        return;
    }
    int octant = (m_bodies->getOctant(node->position) + 8) / 8;
    double octant_width = 1000 / octant; // 1000 is the hardcoded width of the head octant. Change this to be setable by the user?
    glm::dvec3 center_of_mass = calculate_center_of_mass(tree);
    glm::dvec3 distance = center_of_mass - node->position;

    if (octant_width / glm::length(distance) < 0.5) { // then can be treated as a single body
        node->force += calculate_force(node, OctreeNode(center_of_mass, node->velocity, node->force, mass_from_body_cluster(tree)));
        return;
    } else {
        for (int i = 0; i < 8; i++) {
            calculate_force_and_acceleration(tree->getChildren()[i], node);
        }
    }
}

void NBody::update_position(OctreeNode &node, double time)
{
    node.acceleration = glm::dvec3(
        node.force.x / node.mass,
        node.force.y / node.mass,
        node.force.z / node.mass
    );
    node.velocity += node.acceleration * time;
    // std::cerr << "update_position: acceleration (" << node.acceleration.x << ", " << node.acceleration.y << ", " << node.acceleration.z << ")" << std::endl;
    // std::cerr << "update_position: velocity (" << node.velocity.x << ", " << node.velocity.y << ", " << node.velocity.z << ")" << std::endl;
    glm::vec3 new_position = node.position + (node.velocity * time);
    m_bodies->reinsert_at(&node, new_position);
    node.force.x = 0;
    node.force.y = 0;
    node.force.z = 0;
}

Octree* NBody::simulate_frame() // Should return the whole octree so it can be displayed by OpenGL
{
    // move into vector as tree should not change while iterating
    std::vector<OctreeNode *> position_updates_vector;
    for (auto &node : *m_bodies) {
        calculate_force_and_acceleration(m_bodies, &node); // only if node is a body. Figure that out.
        position_updates_vector.push_back(&node);
    }

    for (auto *node : position_updates_vector) update_position(*node);
    return m_bodies;
}
