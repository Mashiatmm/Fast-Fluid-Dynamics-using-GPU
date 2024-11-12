#version 330

out vec2 coords;
layout (location = 0) in vec3 aPos;      // Vertex position
layout (location = 1) in vec2 aTexCoord; //

void main() {
    gl_Position = vec4(aPos, 1.0); // Transform to clip space
    coords = aTexCoord;  
}