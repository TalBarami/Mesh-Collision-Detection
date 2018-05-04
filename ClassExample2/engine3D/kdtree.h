/*
*
* k-d tree declaration file
*	
* author: Arvind Rao	
* license: GNU
* 
*
*/

#ifndef KDTREE_
#define KDTREE_
#define _USE_MATH_DEFINES

#include <memory>
#include <vector>
#include <list>
#include <cmath>
#include <queue>
#include <utility>
#include <glm/glm.hpp>
#include "obj_loader.h"

const int N = 3;
const glm::vec3 x_axis = glm::vec3(1, 0, 0);
const glm::vec3 y_axis = glm::vec3(0, 1, 0);
const glm::vec3 z_axis = glm::vec3(0, 0, 1);

class Node
{
 public:
	Node* left ;// = std::unique_ptr<Node>( new Node(3) );
	Node* right; //( new Node(3));
	glm::vec4 position; 

	float width, height, depth;
	glm::vec3 x, y, z;

	//default constructor
	Node(int _n);

	//copy constructor
	Node(int _n, glm::vec4 _data);

	//default deconstructor
	~Node();

	bool is_leaf();
	bool is_smallest_box();

	glm::vec3 operator[] (const int i) const
	{
		if (i == 0) return x;
		if (i == 1) return y;
		if (i == 2) return z;
		return glm::vec3(0);
	}

	float operator() (const int i) const
	{
		if (i == 0) return width;
		if (i == 1) return height;
		if (i == 2) return depth;
		return 0.0f;
	}
};

class Kdtree
{

 public:
	//default constructor
	Kdtree();

	//default deconstructor
	~Kdtree();
	
	/* 
	*   Return the tree root node
	*/
	Node* getRoot() const { return root; };
	/*
	* support function for printTree
	*/
	void print_data(glm::vec4 pt);
		
	/*  prints the tree
	*   and really works best for small trees 
	*   as a test of tree construction.
	*/  
	void print_tree( Node* head );
	
	//function for finding the median of a list of points
	glm::vec4 find_median(int axis, std::list<glm::vec4> &plist, std::list<glm::vec4> &left, 
								std::list<glm::vec4> &right );
	//function for making the tree
	void make_tree( std::list<glm::vec4> &plist );

	void make_tree(IndexedModel indexed_model);
	
	
 private:
	//helper for makeTree 
	void make_tree(Node* head, std::list<glm::vec4> &plist, int depth);
	void build_box(Node* head, std::list<glm::vec4> &plist);
	static glm::vec3 center(glm::vec3 &p1, glm::vec3 &p2);
	Node* root;
};


#endif