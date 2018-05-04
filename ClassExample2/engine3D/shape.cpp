#include <iostream>
#include "shape.h"
#include <includes/GL/glew.h>


Shape::Shape(const Shape& shape)
{
	if (shape.mesh)
		mesh = shape.mesh;
	if (shape.lineMesh)
		lineMesh = shape.lineMesh;
	tex = shape.tex;
	isCopy = true;
}
Shape::Shape(const std::string& fileName) {
	mesh = new Mesh(fileName);
	tex = 0;
	isCopy = false;
}

Shape::Shape(const std::string & fileName, const unsigned maxFaces)
{
	mesh = new Mesh(fileName, maxFaces);
	tex = 0;
	isCopy = false;
}

Shape::Shape(const std::string& fileName,const std::string& textureFileName){
	mesh = new Mesh(fileName); 
	tex = new Texture(textureFileName);
	isCopy = false;
}

Shape::Shape(const std::string & fileName, const std::string & textureFileName, const unsigned maxFaces)
{
	mesh = new Mesh(fileName, maxFaces);
	tex = new Texture(textureFileName);
	isCopy = false;

	this->surrounding_box = create_box(mesh->tree.getRoot(), glm::vec3(0.0f, 0.0f, 1.0f));
}

Shape::Shape(Vertex* vertices, unsigned int numVertices, unsigned int* indices, unsigned int numIndices)
{
	mesh = new Mesh(vertices,numVertices,indices,numIndices);
	tex = 0; 
	isCopy = false;
}

Shape::Shape(LineVertex* vertices, unsigned int numVertices, unsigned int* indices, unsigned int numIndices)
{

	lineMesh = new LineMesh(vertices,numVertices,indices,numIndices);
	tex = 0; 
	isCopy = false;
}


Shape::Shape(Vertex* vertices, unsigned int numVertices, unsigned int* indices, unsigned int numIndices,const std::string& textureFileName)
{
	mesh = new Mesh(vertices,numVertices,indices,numIndices);
	tex = new Texture(textureFileName);
	isCopy = false;
}

void Shape::addMesh(Vertex* vertices, unsigned int numVertices,unsigned int* indices, unsigned int numIndices, const std::string& textureFileName)
{
	mesh = new Mesh(vertices,numVertices,indices,numIndices);
	tex = new Texture(textureFileName);
}

void Shape::addMesh(Vertex* vertices, unsigned int numVertices,unsigned int* indices, unsigned int numIndices)
{
	mesh = new Mesh(vertices,numVertices,indices,numIndices);
}

void Shape::addLineMesh(LineVertex* vertices, unsigned int numVertices,unsigned int* indices, unsigned int numIndices)
{
	lineMesh = new LineMesh(vertices,numVertices,indices,numIndices);
}

void Shape::draw(int mode)
{
	//transformations
	if (tex)
	{
		tex->Bind();
	}

	if (mesh)
	{
		mesh->Draw(mode);
		if(surrounding_box)
		{
			surrounding_box->Draw(GL_LINE_LOOP);
		}
		if(collision_box)
		{
			collision_box->Draw(GL_LINE_LOOP);
		}
	}

	if (lineMesh)
	{
		lineMesh->Draw();
	}
}

Shape::~Shape(void)
{
	if(!isCopy)
	{
		if(mesh)
 			delete mesh;
		if(lineMesh)
			delete lineMesh;
		if(tex)
			delete tex;
	}
}

bool Shape::collides_with(Shape* other)
{
	Kdtree &t1 = this->mesh->tree;
	Kdtree &t2 = other->mesh->tree;
	std::vector<std::pair<Node*, Node*>> to_check;
	to_check.emplace_back(t1.getRoot(), t2.getRoot());

	while (!to_check.empty())
	{
		auto current = to_check.back();
		to_check.pop_back();

		if (nodes_collide(current.first, current.second, glm::vec3(this->getTraslate()), glm::vec3(other->getTraslate())))
		{
			if (current.first->is_smallest_box() && current.second->is_smallest_box())
			{
				this->set_collision_box(current.first);
				other->set_collision_box(current.second);
				return true;
			}

			push_child_nodes(current, to_check);
		}
	}

	this->set_collision_box(nullptr);
	other->set_collision_box(nullptr);
	return false;
}

bool Shape::nodes_collide(Node* a, Node* b, const glm::vec3& translate_a, const glm::vec3& translate_b) const
{
	glm::vec3 t = (glm::vec3(a->position) + translate_a) - (glm::vec3(b->position) + translate_b);

	float r[3][3];
	float tA[3], tB[3];
	for(int i=0; i<3; i++)
	{
		for(int j=0; j<3; j++)
		{
			r[i][j] = glm::dot((*a)[i], (*b)[j]);
		}
		tA[i] = glm::dot(t, (*a)[i]);
		tB[i] = glm::dot(t, (*b)[i]);
	}

	return !(
		abs(tA[0]) > a->width + abs(b->width * r[0][0]) + abs(b->height * r[0][1]) + abs(b->depth * r[0][2]) ||
		abs(tA[1]) > a->height + abs(b->width * r[1][0]) + abs(b->height * r[1][1]) + abs(b->depth * r[1][2]) ||
		abs(tA[2]) > a->depth + abs(b->width * r[2][0]) + abs(b->height * r[2][1]) + abs(b->depth * r[2][2]) ||
		abs(tB[0]) > b->width + abs(a->width * r[0][0]) + abs(a->height * r[1][0]) + abs(a->depth * r[2][0]) ||
		abs(tB[1]) > b->height + abs(a->width * r[0][1]) + abs(a->height * r[1][1]) + abs(a->depth * r[2][1]) ||
		abs(tB[2]) > b->depth + abs(a->width * r[0][2]) + abs(a->height * r[1][2]) + abs(a->depth * r[2][2]) ||
		abs(tA[2] * r[1][0] - tA[1] * r[2][0]) > abs(a->height * r[2][0]) + abs(a->depth * r[1][0]) + abs(b->height * r[0][2]) + abs(b->depth * r[0][1]) ||
		abs(tA[2] * r[1][1] - tA[1] * r[2][1]) > abs(a->height * r[2][1]) + abs(a->depth * r[1][1]) + abs(b->width * r[0][2]) + abs(b->depth * r[0][0]) ||
		abs(tA[2] * r[1][2] - tA[1] * r[2][2]) > abs(a->height * r[2][2]) + abs(a->depth * r[1][2]) + abs(b->width * r[0][1]) + abs(b->height * r[0][0]) ||
		abs(tA[0] * r[2][0] - tA[2] * r[0][0]) > abs(a->width * r[2][0]) + abs(a->depth * r[0][0]) + abs(b->height * r[1][2]) + abs(b->depth * r[1][1]) ||
		abs(tA[0] * r[2][1] - tA[2] * r[0][1]) > abs(a->width * r[2][1]) + abs(a->depth * r[0][1]) + abs(b->width * r[1][2]) + abs(b->depth * r[1][0]) ||
		abs(tA[0] * r[2][2] - tA[2] * r[0][2]) > abs(a->width * r[2][2]) + abs(a->depth * r[0][2]) + abs(b->width * r[1][1]) + abs(b->height * r[1][0]) ||
		abs(tA[1] * r[0][0] - tA[0] * r[1][0]) > abs(a->width * r[1][0]) + abs(a->height * r[0][0]) + abs(b->height * r[2][2]) + abs(b->depth * r[2][1]) ||
		abs(tA[1] * r[0][1] - tA[0] * r[1][1]) > abs(a->width * r[1][1]) + abs(a->height * r[0][1]) + abs(b->width * r[2][2]) + abs(b->depth * r[2][0]) ||
		abs(tA[1] * r[0][2] - tA[0] * r[1][2]) > abs(a->width * r[1][2]) + abs(a->height * r[0][2]) + abs(b->width * r[2][1]) + abs(b->height * r[2][0])
		);
}

void Shape::push_child_nodes(std::pair<Node*, Node*> p, std::vector<std::pair<Node*, Node*>>& to_check)
{
	if (p.first->is_smallest_box() && p.second != nullptr)
	{
		to_check.emplace_back(p.first, p.second->left);
		to_check.emplace_back(p.first, p.second->right);
	}
	else if (p.second->is_smallest_box() && p.first != nullptr)
	{
		to_check.emplace_back(p.first->left, p.second);
		to_check.emplace_back(p.first->right, p.second);
	}
	else
	{
		to_check.emplace_back(p.first->left, p.second->left);
		to_check.emplace_back(p.first->right, p.second->left);
		to_check.emplace_back(p.first->left, p.second->right);
		to_check.emplace_back(p.first->right, p.second->right);
	}
}

void Shape::set_collision_box(Node* n)
{
	delete collision_box;
	this->collision_box = n != nullptr ? create_box(n, glm::vec3(1.0f, 0.0f, 0.0f)) : nullptr;
}


Mesh* Shape::create_box(Node* n, glm::vec3 color)
{
	float x = n->position.x, y = n->position.y, z = n->position.z;
	float width = n->width, height = n->height, depth = n->depth;

	Vertex vertices[] =
	{
		Vertex(glm::vec3(-width + x , -height + y, -depth + z), glm::vec2(1, 0), glm::vec3(0, 0, -1),color),
		Vertex(glm::vec3(-width + x, height + y, -depth + z), glm::vec2(0, 0), glm::vec3(0, 0, -1),color),
		Vertex(glm::vec3(width + x, height + y, -depth + z), glm::vec2(0, 1), glm::vec3(0, 0, -1),color),
		Vertex(glm::vec3(width + x, -height + y, -depth + z), glm::vec2(1, 1), glm::vec3(0, 0, -1),color),

		Vertex(glm::vec3(-width + x, -height + y, depth + z), glm::vec2(1, 0), glm::vec3(0, 0, 1),color),
		Vertex(glm::vec3(-width + x, height + y, depth + z), glm::vec2(0, 0), glm::vec3(0, 0, 1),color),
		Vertex(glm::vec3(width + x, height + y, depth + z), glm::vec2(0, 1), glm::vec3(0, 0, 1),color),
		Vertex(glm::vec3(width + x, -height + y, depth + z), glm::vec2(1, 1), glm::vec3(0, 0, 1),color),

		Vertex(glm::vec3(-width + x, -height + y, -depth + z), glm::vec2(0, 1), glm::vec3(0, -1, 0),color),
		Vertex(glm::vec3(-width + x, -height + y, depth + z), glm::vec2(1, 1), glm::vec3(0, -1, 0),color),
		Vertex(glm::vec3(width + x, -height + y, depth + z), glm::vec2(1, 0), glm::vec3(0, -1, 0),color),
		Vertex(glm::vec3(width + x, -height + y, -depth + z), glm::vec2(0, 0), glm::vec3(0, -1, 0),color),

		Vertex(glm::vec3(-width + x, height + y, -depth + z), glm::vec2(0, 1), glm::vec3(0, 1, 0),color),
		Vertex(glm::vec3(-width + x, height + y, depth + z), glm::vec2(1, 1), glm::vec3(0, 1, 0),color),
		Vertex(glm::vec3(width + x, height + y, depth + z), glm::vec2(1, 0), glm::vec3(0, 1, 0),color),
		Vertex(glm::vec3(width + x, height + y, -depth + z), glm::vec2(0, 0), glm::vec3(0, 1, 0),color),

		Vertex(glm::vec3(-width + x, -height + y, -depth + z), glm::vec2(1, 1), glm::vec3(-1, 0, 0),color),
		Vertex(glm::vec3(-width + x, -height + y, depth + z), glm::vec2(1, 0), glm::vec3(-1, 0, 0),color),
		Vertex(glm::vec3(-width + x, height + y, depth + z), glm::vec2(0, 0), glm::vec3(-1, 0, 0),color),
		Vertex(glm::vec3(-width + x, height + y, -depth + z), glm::vec2(0, 1), glm::vec3(-1, 0, 0),color),

		Vertex(glm::vec3(width + x, -height + y, -depth + z), glm::vec2(1, 1), glm::vec3(1, 0, 0),color),
		Vertex(glm::vec3(width + x, -height + y, depth + z), glm::vec2(1, 0), glm::vec3(1, 0, 0),color),
		Vertex(glm::vec3(width + x, height + y, depth + z), glm::vec2(0, 0), glm::vec3(1, 0, 0),color),
		Vertex(glm::vec3(width + x, height + y, -depth + z), glm::vec2(0, 1), glm::vec3(1, 0, 0),color)
	};

	unsigned int indices[] = {
		0, 1, 2,
		0, 2, 3,

		6, 5, 4,
		7, 6, 4,

		10, 9, 8,
		11, 10, 8,

		12, 13, 14,
		12, 14, 15,

		16, 17, 18,
		16, 18, 19,

		22, 21, 20,
		23, 22, 20
	};

	return new Mesh(vertices, sizeof(vertices) / sizeof(vertices[0]), indices, sizeof(indices) / sizeof(indices[0]));
}
