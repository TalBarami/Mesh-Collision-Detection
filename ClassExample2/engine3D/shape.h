#pragma once
#include "MovableGLM.h"
#include "mesh.h"
#include "lineMesh.h"
#include "texture.h"
#include <list>
#include "kdtree.h"

class Shape : public MovableGLM
{
	LineMesh *lineMesh;
	Texture *tex;
	bool isCopy;
public:
	Mesh *mesh;
	Mesh *surrounding_box;
	Mesh *collision_box;

	enum{triangles,lines};
	Shape(const Shape& shape);
	Shape(const std::string& fileName);
	Shape(const std::string& fileName, const unsigned maxFaces);
	Shape(const std::string& fileName,const std::string& textureFileName);
	Shape(const std::string& fileName, const std::string& textureFileName, const unsigned maxFaces);
	Shape(Vertex* vertices, unsigned int numVertices, unsigned int* indices, unsigned int numIndices);
	Shape(LineVertex* vertices, unsigned int numVertices, unsigned int* indices, unsigned int numIndices);
	Shape(Vertex* vertices, unsigned int numVertices, unsigned int* indices, unsigned int numIndices, const std::string& textureFileName);
	void addMesh(Vertex* vertices, unsigned int numVertices,unsigned int* indices, unsigned int numIndices, const std::string& textureFileName);
	void addMesh(Vertex* vertices, unsigned int numVertices,unsigned int* indices, unsigned int numIndices);
	void addLineMesh(LineVertex* vertices, unsigned int numVertices,unsigned int* indices, unsigned int numIndices);
	void draw(int mode);
	virtual ~Shape(void);

	bool collides_with(Shape* other);
private:
	bool nodes_collide(Node* a, Node* b, const glm::vec3 &translate_a, const glm::vec3 &translate_b) const;
	static void push_child_nodes(std::pair<Node*, Node*> p, std::vector<std::pair<Node*, Node*>> &to_check);
	void set_collision_box(Node* n);
	static Mesh* create_box(Node* n, glm::vec3 color);
};

