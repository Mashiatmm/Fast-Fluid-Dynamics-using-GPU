#ifndef MESH_HPP
#define MESH_HPP
#include "glad.h"
#include <GLFW/glfw3.h>
#include <string>
#include <vector>
#include <utility>
#include <glm/glm.hpp>
// #include "gl_core_3_3.h"

class Mesh {
public:
	Mesh(std::string filename);
	~Mesh() { release(); }

	// Return the bounding box of this object
	std::pair<glm::vec3, glm::vec3> boundingBox() const
	{ return std::make_pair(minBB, maxBB); }

	void load(std::string filename);
	void draw();

	// Mesh vertex format
	struct Vtx {
		glm::vec3 pos;		// Position
		glm::vec3 norm;		// Normal
	};

protected:
	void release();		// Release OpenGL resources

	// Bounding box
	glm::vec3 minBB;
	glm::vec3 maxBB;

	// OpenGL resources
	unsigned int vao;		// Vertex array object
	unsigned int vbuf;	// Vertex buffer
	unsigned int vcount;	// Number of vertices

private:
	// Disallow copy and move
	Mesh(const Mesh& other);
	Mesh(Mesh&& other);
	Mesh& operator=(const Mesh& other);
	Mesh& operator=(Mesh&& other);
};

#endif