#ifndef CUBE_H
#define CUBE_H

#include "../glad.h"
#include <GLFW/glfw3.h>
#include <chrono>
#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <utility>
#include <glm/glm.hpp>

using namespace std;
using namespace glm;

struct Cube {
    glm::vec3 center;  // Center of the cube
    glm::vec3 size;    // Half-extents of the cube
};

Cube cube = { glm::vec3(0.5, 0.5, 0.5), glm::vec3(0.1, 0.1, 0.1) }; // Example

glm::vec3 minBound = cube.center - cube.size;
glm::vec3 maxBound = cube.center + cube.size;

void drawCube(const Cube& cube) {
  

    GLfloat vertices[] = {
        // Front face
        minBound.x, minBound.y, maxBound.z,
        maxBound.x, minBound.y, maxBound.z,
        maxBound.x, maxBound.y, maxBound.z,
        minBound.x, maxBound.y, maxBound.z,
        // Back face
        minBound.x, minBound.y, minBound.z,
        maxBound.x, minBound.y, minBound.z,
        maxBound.x, maxBound.y, minBound.z,
        minBound.x, maxBound.y, minBound.z,
        // Other faces...
    };

    GLuint indices[] = {
        // Front face
        0, 1, 2, 2, 3, 0,
        // Back face
        4, 5, 6, 6, 7, 4,
        // Other faces...
    };

    GLuint VAO, VBO, EBO;
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);

    glBindVertexArray(VAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    glDrawElements(GL_TRIANGLES, sizeof(indices) / sizeof(GLuint), GL_UNSIGNED_INT, 0);

    glDeleteBuffers(1, &VBO);
    glDeleteBuffers(1, &EBO);
    glDeleteVertexArrays(1, &VAO);
}


#endif