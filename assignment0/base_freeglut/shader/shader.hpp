#ifndef _SHADER_HPP
#define _SHADER_HPP

#include "../glad.h"
#include <GLFW/glfw3.h>
#include <string>
#include <vector>
#include <utility>
#include <glm/glm.hpp>
#include "shader.h"
// #include "gl_core_3_3.h"

// const unsigned int SCR_WIDTH = 800;
// const unsigned int SCR_HEIGHT = 600;


void initShader();
void setShader(unsigned int shader);
void advectionStep();

#endif