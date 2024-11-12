#ifndef _COMMON_HPP
#define _COMMON_HPP

#include "glad.h"
#include <string>
#include <vector>
// #include "gl_core_3_3.h"

std::string loadShaderSource(const char* filepath);
unsigned int compileShader(const char* filepath, GLenum shaderType);
GLuint linkProgram(std::vector<GLuint> shaders);

#endif