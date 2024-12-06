#version 330

out vec2 coords;
layout (location = 0) in vec3 aPos;      // Vertex position
layout (location = 1) in vec2 aTexCoord; //

uniform mat4 xform;			// Model-to-clip space transform


void main() {
    // gl_Position = xform * vec4(aPos, 1.0); // Transform to clip space
    gl_Position = vec4(aPos, 1.0);
    coords = aTexCoord;  
}