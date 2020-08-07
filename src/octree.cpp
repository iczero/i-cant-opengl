#include "octree.hpp"

/*#define TopLeftFront 0 
#define TopRightFront 1 
#define BottomRightFront 2 
#define BottomLeftFront 3 
#define TopLeftBottom 4 
#define TopRightBottom 5 
#define BottomRightBack 6 
#define BottomLeftBack 7*/

OctreeNode::OctreeNode():
    force(0.0),
    mass(0.0),
    parent(nullptr) {}
OctreeNode::OctreeNode(glm::dvec3 position, glm::dvec3 velocity, glm::dvec3 force, double mass):
    position(position),
    velocity(velocity),
    force(force),
    mass(mass),
    parent(nullptr) {}

void OctreeNode::printOctreeNode() {
    std::cout << "Node EXISTS! Position: " << position.x << ", " << position.y << ", " << position.z << std::endl;
}

Octree::Octree(glm::dvec3 half, glm::dvec3 orig):
    origin(orig), halfDim(half), data(nullptr), is_leaf(false)
{
    for (int i = 0; i < 8; ++i) {
        children[i] = nullptr;
    }
}
Octree::Octree(OctreeNode* node, glm::dvec3 half, glm::dvec3 orig):
    origin(orig), halfDim(half), data(node), parent(nullptr), is_leaf(true)
{
    for (int i = 0; i < 8; ++i) {
        children[i] = nullptr;
    }
    if (data) data->parent = this;
}

Octree::~Octree() {
    for (int i = 0; i < 8; ++i) {
        if (children[i]) delete children[i];
    }
    if (data) delete data;
}

// Getters
Octree** Octree::getChildren() { return children; }
OctreeNode* Octree::getData() { return data; }

int Octree::getOctant(glm::dvec3 &point) {
    int octant = 0;
    if (point.x >= origin.x) octant += 4;
    if (point.y >= origin.y) octant += 2;
    if (point.z >= origin.z) octant += 1;
    return octant;
}

// Boolean Checks
bool Octree::isLeafNode()
{
    return is_leaf;
}

bool Octree::bounds_check(glm::dvec3 &position) {
    return (position.x >= (origin.x - halfDim.x) && position.x <= (origin.x + halfDim.x)
        && position.y >= (origin.y - halfDim.y) && position.y <= (origin.y + halfDim.y)
        && position.z >= (origin.z - halfDim.z) && position.z <= (origin.z + halfDim.z));
}

// insert node into tree
bool Octree::insert(OctreeNode *node) {
    // check if node is out of bounds 
    if (!bounds_check(node->position)) return false;

    if (is_leaf) {
        // return if leaf and data not yet assigned
        if (!data) {
            data = node;
            node->parent = this;
            return true;
        }

        if (node == data) {
            throw std::logic_error("attempted to reinsert the same node twice!");
        }
        if (data->position == node->position) {
            // throw std::runtime_error("two nodes with identical positions exist");
            std::cerr << "warning: attempted to insert 2 nodes at the same position" << std::endl;
            return false;
        }

        // convert to internal node
        is_leaf = false;
        OctreeNode *old_data = data;
        data = nullptr;
        old_data->parent = nullptr;
        insert_non_leaf(old_data);        
        // fall through
    }

    // recursively insert
    return insert_non_leaf(node);
}

bool Octree::insert_non_leaf(OctreeNode *node) {
    // at interior node, insert recursively into appropriate child octant
    int target_octant = getOctant(node->position);
    if (!children[target_octant]) {
        glm::dvec3 newOrigin = origin;

        // compute new bounding box for child
        if (target_octant & 4)	// calculate X
        {
            newOrigin.x += halfDim.x * 0.5;
        }
        else
        {
            newOrigin.x += halfDim.x * -0.5;
        }

        if (target_octant & 2)	// calculate Y
        {
            newOrigin.y += halfDim.y * 0.5;
        }
        else
        {
            newOrigin.y += halfDim.y * -0.5;
        }

        if (target_octant & 1)	// calculate Z
        {
            newOrigin.z += halfDim.z * 0.5;
        }
        else
        {
            newOrigin.z += halfDim.z * -0.5;
        }
        
        glm::dvec3 half = halfDim / 2.0;
        Octree *new_tree = new Octree(nullptr, half, newOrigin);
        new_tree->parent = this;
        children[target_octant] = new_tree;
    }
    return children[target_octant]->insert(node);
}

bool Octree::has_children() {
    for (auto i = 0U; i < 8; i++) {
        if (children[i] != nullptr) return true;
    }
    return false;
}

bool Octree::remove(OctreeNode* node) {
    if (is_leaf) {
        if (node == data) {
            data->parent = nullptr;
            data = nullptr;
            remove_self(); // we are now empty
            return true;
        } else return false;
    } else {
        int target_octant = getOctant(node->position);
        if (!children[target_octant]) {
            // getOctant has accuracy problems sometimes????
            // try using node->parent instead
            if (!node->parent) return false;
            bool fallback_succeeded = false;
            for (auto i = 0U; i < 8; i++) {
                if (node->parent == children[i]) {
                    target_octant = i;
                    fallback_succeeded = true;
                    break;
                }
            }
            if (!fallback_succeeded) return false;
        }
        if (children[target_octant]->remove(node)) {
            // removed successfully, check if node is now empty
            if (!has_children()) remove_self();
            return true;
        } else return false;
    }
}

void Octree::remove_self() {
    if (!parent) return;
    int target_octant = parent->getOctant(origin);
    if (!parent->children[target_octant]) throw std::runtime_error("tree is broken");
    if (parent->children[target_octant] != this) throw std::runtime_error("tree is broken");
    // detach self
    parent->children[target_octant] = nullptr;
    // commit suicide
    // probably not a good idea
    delete this;
}

void Octree::reinsert_at(OctreeNode *node, glm::dvec3 new_position) {
    if (!node->parent) {
        // was never in the tree in the first place
        node->position = new_position;
        insert(node);
        return;
    }

    {
        Octree &parent = *node->parent;
        if (parent.bounds_check(new_position)) {
            // node does not need to be reinserted
            node->position = new_position;
            return;
        }
        // remove() may deallocate parent, drop reference here
    }

    // remove and reinsert
    if (!remove(node)) std::cerr << "warning: reinsert failed to remove old node" << std::endl;
    node->position = new_position;
    insert(node);
}

OctreeIterator Octree::begin() {
    return OctreeIterator(this);
}

OctreeIterator Octree::end() {
    return OctreeIterator(nullptr);
}

OctreeIterator::OctreeIterator(Octree *target): current(target), next_output(nullptr) {
    tree_visit_stack.push_back(current);
    if (target) next_output = next();
}

OctreeNode *OctreeIterator::next() {
    // crappy state machine
    while (true) {
        if (node_visit_stack.size()) {
            // return next child node
            OctreeNode *out = node_visit_stack.back();
            node_visit_stack.pop_back();
            return out;
        } else if (tree_visit_stack.size()) {
            Octree *tree = tree_visit_stack.back();
            tree_visit_stack.pop_back();
            if (tree->is_leaf) {
                current = tree;
                if (!tree->data) continue;
                node_visit_stack.push_back(tree->data);
                continue;
            }
            for (auto i = 7; i >= 0; i--) {
                Octree *subtree = tree->children[i];
                if (!subtree) continue;
                tree_visit_stack.push_back(subtree);
            }
            continue;
        } else {
            // we're done
            current = nullptr;
            return nullptr;
        }
    }
}

OctreeIterator OctreeIterator::operator++() {
    next_output = next();
    return *this;
}

OctreeNode &OctreeIterator::operator*() {
    return *next_output;
}

bool OctreeIterator::operator!=(OctreeIterator &other) {
    // note: this ONLY checks whether or not the iterator ended
    return current != other.current;
}

void Octree::printOctree(int level)
{
    //cout << "CHECK";
    if (data)
    {
        std::cout << "Level " << level << ":" << std::endl;
        data->printOctreeNode();
    }
    else
    {
        std::cout << "Level " << level << ":" << std::endl;
        std::cout << "Node is NULL!" << std::endl;
    }

    for (int i = 0; i < 8; ++i)
    {
        if (children[i])
        {
            children[i]->printOctree(level + 1);
        }
    }
}
