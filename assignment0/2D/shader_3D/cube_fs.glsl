#version 330

out vec4 FragColor;

uniform vec3 cubeColor; 

void main() {
    FragColor = vec4(cubeColor, 1.0); 
}
