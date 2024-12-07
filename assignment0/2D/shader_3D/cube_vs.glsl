#version 330
layout(location = 0) in vec3 aPos;
uniform mat4 xform;

void main() {
    gl_Position =  xform * vec4(aPos, 1.0);
}
