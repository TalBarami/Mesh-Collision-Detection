/**
*
* k-d tree implementation file
*	
* author: Arvind Rao	
* license: GNU
* 
*
*/
#include "kdtree.h"
#include <iostream>

// implementation of Node Class

Node::Node(int _n) : x(x_axis), y(y_axis), z(z_axis){
	left = nullptr;
	right = nullptr;
}
Node::Node(int _n, glm::vec4 position) : position(position), x(x_axis), y(y_axis), z(z_axis)
{
	left = nullptr;
	right = nullptr;
}

Node::~Node(){}

bool Node::is_leaf()
{
	return left == nullptr && right == nullptr;
}

bool Node::is_smallest_box()
{
	return !is_leaf() && (left == nullptr || left->is_leaf()) && (left == nullptr || left->is_leaf());
}


// implementation of kdtree

Kdtree::Kdtree(){}
Kdtree::~Kdtree(){}

/*
*
*
*/
glm::vec4 Kdtree::find_median(int axis, std::list<glm::vec4> &plist, std::list<glm::vec4> &left, 
							std::list<glm::vec4> &right )
{
	int size = plist.size();
	int med = ceil( float(size) / float(2) );
	int count = 0;
	
	if ( size == 1)
		return plist.front();
	
	// Using lambda function here, to define comparison function--parametrized by 'axis'
	plist.sort( [&](glm::vec4& a, glm::vec4& b){return a[axis] < b[axis];});
	
	for ( auto& x : plist )
	{
		if (count < med)
			left.push_back(x);
		else
			right.push_back(x);
		++count;
	}
	glm::vec4 median = left.back();
	left.pop_back();
	return median;
}

void Kdtree::print_data(glm::vec4 pt)
{
	for( int i = 0; i < N; i++)
	{
	    std::cout<< pt[i] << ", ";
	}
	std::cout<<"\n";
}

/*
*
*
*/
void Kdtree::print_tree( Node* head )
{
	//find the tree depth 
	int maxdepth = 3;
	int spaces = pow(2, maxdepth +1 ) - 1;
	int depth = 0;
	
	std::cout<< "\n**** Print of Tree **********\n";
	std::queue< Node* > current, next; 
	Node * temp = head;
	current.push(temp);

	while( !current.empty() )
	{
		temp = current.front();
		current.pop();

		if (temp == nullptr)
			std::cout<<"NULL\n";
		else
		{
			Kdtree::print_data(temp->position);
			next.push(temp->left);
			next.push(temp->right);
		}
		if(current.empty())
		{
			depth++;
			std::cout<< "level: "<<  depth <<"\n";
			std::swap(current, next);
		}
	}
}

/*
*  algorithm is based on http://en.wikipedia.org/wiki/Kd_tree
*/
void Kdtree::make_tree(std::list<glm::vec4>& plist)
{
	Node* head = new Node(3);
	Kdtree::make_tree(head, plist, 0);
	Kdtree::root = head;
}

void Kdtree::make_tree(IndexedModel indexed_model)
{
	std::list<glm::vec4> vertices;
	for (auto& position : indexed_model.positions)
	{
		vertices.push_back(glm::vec4(position, 1));
	}
	make_tree(vertices);
}

void Kdtree::make_tree( Node* head, std::list<glm::vec4>& plist, int depth )
{	
	if( !plist.empty() ) 
	{
		int k = N;
		int axis = depth % k;
		
		std::list<glm::vec4> left_list;
		std::list<glm::vec4> right_list;
		glm::vec4 median = Kdtree::find_median(axis, plist, left_list, right_list); 
		head->position = median;
		build_box(head, plist);

		Node* left_node = new Node(k);
		Node* right_node = new Node(k);

		Kdtree::make_tree( left_node, left_list, depth+1);
		if (!left_list.empty()) head->left = left_node;
		
		Kdtree::make_tree( right_node, right_list, depth+1);
		if (!right_list.empty()) head->right = right_node;
	}
}

void Kdtree::build_box(Node* head, std::list<glm::vec4> &plist)
{
	glm::vec3 min = glm::vec3(FLT_MAX, FLT_MAX, FLT_MAX);
	glm::vec3 max = glm::vec3(-FLT_MAX, -FLT_MAX, -FLT_MAX);

	for (auto& vertex : plist)
	{
		for(auto i=0; i<3; i++)
		{
			if(vertex[i] < min[i])
			{
				min[i] = vertex[i];
			}
			if(vertex[i] > max[i])
			{
				max[i] = vertex[i];
			}
		}
	}
	head->position = glm::vec4(center(min, max), 1);
	head->width = head->position.x - min.x;
	head->height = head->position.y - min.y;
	head->depth = head->position.z - min.z;
	
}

glm::vec3 Kdtree::center(glm::vec3& p1, glm::vec3& p2)
{
	return glm::vec3((p1.x + p2.x) / 2, (p1.y + p2.y) / 2, (p1.z + p2.z) / 2);
}