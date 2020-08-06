#pragma once

#include <iostream> 
#include <algorithm>
#include <vector> 
#include <math.h>

using namespace std;

/*#define TopLeftFront 0 
#define TopRightFront 1 
#define BottomRightFront 2 
#define BottomLeftFront 3 
#define TopLeftBottom 4 
#define TopRightBottom 5 
#define BottomRightBack 6 
#define BottomLeftBack 7*/

struct Vec3
{
	float x, y, z;
	Vec3()
	{
		x, y, z = -1;
	}
	Vec3(const Vec3& other)
	{
		x = other.x;
		y = other.y;
		z = other.z;
	}
	Vec3(float x, float y, float z)
	{
		this->x = x;
		this->y = y;
		this->z = z;
	}
	
	operator float* ()
	{
		return &x;
	}
	operator const float* () const
	{
		return &x;
	}

	Vec3& operator=(const Vec3& other) 
	{ 
		x = other.x;
		y = other.y;
		z = other.z;
		return *this; 
	}
	Vec3 operator+(Vec3& other) const
	{
		return Vec3(x + other.x, y + other.y, z + other.z);
	}
    Vec3& operator+=(Vec3&& other)
    {
        x += other.x;
        y += other.y;
        z += other.z;
        return *this;
    }
    Vec3& operator+=(Vec3& other)
    {
        x += other.x;
        y += other.y;
        z += other.z;
        return *this;
    }
	Vec3 operator-(Vec3& other) const
	{
		return Vec3(x - other.x, y - other.y, z - other.z);
	}
	Vec3 operator*(float other) const
	{
		return Vec3(x * other, y * other, z * other);
	}
	Vec3 operator/(float other) const
	{
		return Vec3(x / other, y / other, z / other);
	}

	bool operator>(const Vec3& other) const
	{
		return x > other.x&& y > other.y&& z > other.z;
	}
	bool operator<(const Vec3& other) const
	{
		return x < other.x&& y < other.y&& z < other.z;
	}
	bool operator<=(const Vec3& other) const
	{
		return x <= other.x && y <= other.y && z <= other.z;
	}
	bool operator>=(const Vec3& other) const
	{
		return x >= other.x && y >= other.y && z >= other.z;
	}
	bool operator==(const Vec3& other) const
	{
		return x == other.x && y == other.y && z == other.z;
	}

	// Dot Product
	float operator*(const Vec3& other) const
	{
		return x * other.x + y * other.y + z * other.z;
	}
	float norm() const
	{
		return sqrtf(x * x + y * y + z * z);
	}
	float normSquared() const
	{
		return x * x + y * y + z * z;
	}
};

struct OctreeNode
{
	Vec3 position; // used for insertions
	Vec3 velocity;
    Vec3 acceleration;
	Vec3 force;
	float radius;
	float mass;

	// Constructors
	OctreeNode() {	}
	OctreeNode(Vec3 position, Vec3 velocity, Vec3 force)
	{
		this->position = position;
		this->velocity = velocity;
		this->force = force;
	}
	OctreeNode(Vec3 position, Vec3 velocity, Vec3 force, float radius, float mass)
	{
		this->position = position;
		this->velocity = velocity;
		this->force = force;
		this->radius = radius;
		this->mass = mass;
	}

	// Operators
	bool equals(OctreeNode* node)
	{
		if (this->position == node->position
			&& this->velocity == node->velocity
			&& this->force == node->force
			&& this->radius == node->radius)
		{
			return true;
		}
		else
		{
			return false;
		}
	}

	void printOctreeNode()
	{
		cout << "Node EXISTS! Position: " << position.x << ", " << position.y << ", " << position.z << endl;
	}

	// Getters
	Vec3* getPosition()
	{
		return &position;
	}
	Vec3* getVelocity()
	{
		return &velocity;
	}
	Vec3* getForce()
	{
		return &force;
	}

	// Setters
	void setPosition(const Vec3& p)
	{
		position = p;
	}
	void setVelocity(const Vec3& v)
	{
		velocity = v;
	}
	void setForce(const Vec3& f)
	{
		force = f;
	}
};

class Octree
{
private:
	Vec3* origin;
	Vec3* halfDim;
	Vec3* topLeftFront;
	Vec3* bottomRightBack;
	OctreeNode* data;
	Octree* children[8] { 0 };

public:
	vector<OctreeNode*> allNodes;

	// Big Three
	Octree(OctreeNode* node, Vec3* half, Vec3* orig)
	{
		halfDim = half;
		origin = orig;
		//topLeftFront = new Vec3(orig->x - half->x, orig->y - half->y, orig->z - half->z);	 // min
		//bottomRightBack = new Vec3(orig->x + half->x, orig->y + half->y, orig->z + half->z); // max
		data = node;
		for (int i = 0; i < 8; ++i)
		{
			children[i] = nullptr;
		}
	}
	Octree(const Octree& copy) // double check parameter
	{
		halfDim = copy.halfDim;
		origin = copy.origin;
		/*topLeftFront = new Vec3(copy.origin->x - copy.halfDim->x,
								copy.origin->y - copy.halfDim->y,
								copy.origin->z - copy.halfDim->z);	 // min
		bottomRightBack = new Vec3(copy.origin->x + copy.halfDim->x, 
								copy.origin->y + copy.halfDim->y,
								copy.origin->z + copy.halfDim->z); // max*/
		data = copy.data;
	}
	~Octree()
	{
		for (int i = 0; i < 8; ++i)
		{
			delete children[i];
		}
	}
	// Getters
    Octree** getChildren() { return children; }
    OctreeNode* getData() { return data; }
	/*int getAverageX()
	{
		return topLeftFront->x + bottomRightBack->x / 2;
	}

	int getAverageY()
	{
		return topLeftFront->y + bottomRightBack->y / 2;
	}

	int getAverageZ()
	{
		return topLeftFront->z + bottomRightBack->z / 2;
	}*/

	int getOctant(Vec3* point)
	{
		int octant = 0;
		if (point->x >= origin->x)
		{
			octant += 4;
		}
		if (point->y >= origin->y)
		{
			octant += 2;
		}
		if (point->z >= origin->z)
		{
			octant += 1;
		}
		return octant;
	}

	// Boolean Checks
	bool isLeafNode()
	{
		// leaf iff has no children
		for (int i = 0; i < 8; ++i)
		{
			if (children[i] != nullptr)
			{
				return false;
			}
		}
		return true;
	}

	bool nodeExists(OctreeNode* node) // Search
	{
		bool ret = false;
		if (data && node->equals(data))
		{
			ret = true;
		}
		else
		{
			for (int i = 0; i < 8; ++i)
			{
				if (children[i])
				{
					ret |= children[i]->nodeExists(node);
				}
			}
		}
		return ret;
	}
	
	// insert node into tree
	void insert(OctreeNode* node)
	{
		// check if it node not in vector
		if (find(allNodes.begin(), allNodes.end(), node) == allNodes.end()) 
		{
			allNodes.push_back(node);
		}
		
		// check if node exists
		if (nodeExists(node))
		{
			return;
		}
		
		// check if node is out of bounds 
		if (   node->position.x < (origin->x - halfDim->x) || node->position.x >(origin->x + halfDim->x)
			|| node->position.y < (origin->y - halfDim->y) || node->position.y >(origin->y + halfDim->y)
			|| node->position.z < (origin->z - halfDim->z) || node->position.z >(origin->z + halfDim->z))
		{
			return;
		}

		if (isLeafNode())
		{
			// return if leaf and data not yet assigned
			if (data == nullptr)
			{
				data = node;
				// cout << "data is nullptr" << endl;
				return;
			}
			// split node to have 8 child octants then insert data
			else
			{
				// save data of old node for later
				OctreeNode* oldNode = data;
				data = nullptr;

				// split current node and create empty trees for each child octant
				for (int i = 0; i < 8; ++i)
				{
					Vec3* newOrigin = new Vec3(*origin);

					// compute new bounding box for child
					if (i & 4)	// calculate X
					{
						newOrigin->x += halfDim->x * 0.5f;
					}
					else
					{
						newOrigin->x += halfDim->x * -0.5f;
					}

					if (i & 2)	// calculate Y
					{
						newOrigin->y += halfDim->y * 0.5f;
					}
					else
					{
						newOrigin->y += halfDim->y * -0.5f;
					}

					if (i & 1)	// calculate Z
					{
						newOrigin->z += halfDim->z * 0.5f;
					}
					else
					{
						newOrigin->z += halfDim->z * -0.5f;
					}

					Vec3* half = new Vec3(*halfDim * 0.5f);
					children[i] = new Octree(nullptr, half, newOrigin);
				}

				// re-insert old and new node
				Vec3* b1 = oldNode->getPosition();
				int b2 = getOctant(oldNode->getPosition());

				children[getOctant(oldNode->getPosition())]->insert(oldNode);
				children[getOctant(node->getPosition())]->insert(node);
			}
		}
		else
		{
			// at interior node, insert recursively into appropriate child octant
			children[getOctant(node->getPosition())]->insert(node);
		}
	}

	Octree* remove(OctreeNode* node)
	{
		Octree* newTree = new Octree(nullptr, halfDim, origin);
		vector<OctreeNode*> newNodes;

		for (int i = 0; i < allNodes.size(); i++)
		{
			if (!(node->equals(allNodes[i])))
			{
				newTree->insert(allNodes[i]);
				newNodes.push_back(allNodes[i]);
				// cout << i << " adding " << allNodes[i] << endl;
			}
		}
		allNodes = newNodes;
		return newTree;
	}

	/*int getNumNodes()
	{
		int numNodes = 0;
		if (this->data)
		{
			numNodes += 1;
		}
		else
		{
			for (int i = 0; i < 8; ++i)
			{
				if (children[i])
				{
					numNodes += children[i]->getNumNodes();
				}
			}
		}
		return numNodes;
	}

	OctreeNode* getAllNodes(int numNodes)
	{
		OctreeNode nodes[numNodes];
		if (this->data)
		{
			numNodes += 1;
		}
		else
		{
			for (int i = 0; i < 8; ++i)
			{
				if (children[i])
				{
					numNodes += children[i]->getNumNodes();
				}
			}
		}
		return numNodes;
	}*/

	// get information inside nodes
	/*void checkNodes( Vec3& min, Vec3& max, vector<OctreeNode*>& nodes)
	{
		// if a leaf node, check the current data inside
		if (isLeafNode())
		{
			if (data != nullptr)
			{
				Vec3* pos = data->getPosition();
				Vec3* vel = data->getVelocity();
				Vec3* frc = data->getForce();

				// return if position out of bounds
				if (pos.x < min.x || pos.y < min.y || pos.z < min.z)
				{
					return;
				}
				if (pos.x > max.x || pos.y > max.y || pos.z > max.z)
				{
					return;
				}
			}
		}
		else
		{
			// at interior node of the tree
			for (int i = 0; i < 8; ++i)
			{
				// calculate min and max of child octant
				Vec3 childMax = children[i]->origin + children[i]->halfDim;
				Vec3 childMin = children[i]->origin - children[i]->halfDim;

				// continue if outside child's bounds
				if (childMax.x < min.x || childMax.y < min.y || childMax.z < min.z)
				{
					continue;
				}
				if (childMin.x > max.x || childMin.y > max.y || childMin.z > max.z)
				{
					continue;
				}

				// recursive loop once determined child is intersecting bounding box
				children[i]->checkNodes(min, max, nodes);
			}
		}
	}*/

	void printOctree(int level)
	{
		//cout << "CHECK";
		if (data)
		{
			cout << "Level " << level << ":" << endl;
			data->printOctreeNode();
		}
		else
		{
			cout << "Level " << level << ":" << endl;
			cout << "Node is NULL!" << endl;
		}

		for (int i = 0; i < 8; ++i)
		{
			if (children[i])
			{
				children[i]->printOctree(level + 1);
			}
		}
	}

	/*bool isInBounds(const Vec3& point, const Vec3& bmin, const Vec3& bmax) {
		return
			point.x >= bmin.x &&
			point.y >= bmin.y &&
			point.z >= bmin.z &&
			point.x <= bmax.x &&
			point.y <= bmax.y &&
			point.z <= bmax.z;
	}*/
};

/*
int main()
{
	// std::cout.flush();

	// create initial data and node
	Vec3* pos = new Vec3(1, 2, 3);
	Vec3* vel = new Vec3(4, 5, 6);
	Vec3* frc = new Vec3(7, 8, 9);
	OctreeNode* node = new OctreeNode(*pos, *vel, *frc);

	// create octree
	Vec3* half = new Vec3(50, 50, 50);
	Vec3* origin = new Vec3(50, 50, 50);
	Octree* tree = new Octree(node, half, origin);

	// create new node
	Vec3* pos1 = new Vec3(99, 99, 99);
	Vec3* vel1 = new Vec3(2, 2, 2);
	Vec3* frc1 = new Vec3(4, 4, 4);
	OctreeNode* newNode1 = new OctreeNode(*pos1, *vel1, *frc1);

	// create new node 2
	Vec3* pos2 = new Vec3(20, 20, 20);
	Vec3* vel2 = new Vec3(10, 27, 32);
	Vec3* frc2 = new Vec3(5, 5, 4);
	OctreeNode* newNode2 = new OctreeNode(*pos2, *vel2, *frc2);

	// create new node 3
	Vec3* pos3 = new Vec3(10, 20, 80);
	Vec3* vel3 = new Vec3(1, 1, 60);
	Vec3* frc3 = new Vec3(9, 9, 20);
	OctreeNode* newNode3 = new OctreeNode(*pos3, *vel3, *frc3);

	// insert new node
	tree->insert(newNode1);
	tree->insert(newNode2);
	tree->insert(newNode3);

	// Print Tree
	tree->printOctree(0);
	cout << "--------------------------------------------------------------" << endl << endl;

	// Remove Node from tree
	for (int i = 0; i < allNodes.size(); ++i)
	{
		cout << allNodes[i] << endl;
	}
	cout << endl;

	tree = tree->remove(newNode2);

	for (int i = 0; i < allNodes.size(); ++i)
	{
		cout << allNodes[i] << endl;
	}

	// Print Tree
	tree->printOctree(0);
	cout << "--------------------------------------------------------------" << endl << endl;
	return 0;
}
*/
