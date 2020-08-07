#pragma once
#include <iostream> 
#include <algorithm>
#include <vector> 
#include <math.h>
#include <glm/glm.hpp>

// forward declaration
class Octree;

struct OctreeNode
{
	glm::dvec3 position; // used for insertions
	glm::dvec3 velocity;
    glm::dvec3 acceleration;
	glm::dvec3 force;
	double mass;
	// parent tree containing this node
	Octree *parent;

	// Constructors
	OctreeNode();
	OctreeNode(glm::dvec3 position, glm::dvec3 velocity, glm::dvec3 force, double mass = 0.0);

	void printOctreeNode();
};

class OctreeIterator {
public:
	Octree *current;
	OctreeNode *next_output;
	std::vector<Octree *> tree_visit_stack;
	std::vector<OctreeNode *> node_visit_stack;

	OctreeIterator(Octree *target);
	OctreeNode *next();
	OctreeIterator operator++();
	OctreeNode &operator*();
	bool operator!=(OctreeIterator &other);
};

class Octree
{
	friend class OctreeIterator;
private:
	glm::dvec3 origin;
	glm::dvec3 halfDim;
	OctreeNode* data;
	Octree* children[8] { 0 };
	Octree *parent;
	bool is_leaf;

public:
	Octree(glm::dvec3 half, glm::dvec3 orig);
	Octree(OctreeNode* node, glm::dvec3 half, glm::dvec3 orig);
	~Octree();

	// Getters
    Octree** getChildren();
    OctreeNode* getData();

	int getOctant(glm::dvec3 &point);

	// Boolean Checks
	bool isLeafNode();

	bool bounds_check(glm::dvec3 &position);
	bool bounds_check_strict(glm::dvec3 &position);
	
	// insert node into tree
	bool insert(OctreeNode *node);
	// don't use this unless you know what you're doing
	bool insert_non_leaf(OctreeNode *node);

	// recursively remove nodes, but will not deallocate node
	// should only be called on the topmost node
	bool remove(OctreeNode* node);
	void remove_self();
	bool has_children();

	// fast remove followed by reinsert
	void reinsert_at(OctreeNode *node, glm::dvec3 new_position);

	void printOctree(int level);

	// iteration
	OctreeIterator begin();
	OctreeIterator end();
};
