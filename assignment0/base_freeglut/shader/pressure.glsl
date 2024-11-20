#version 330 core

out vec4 fragColor;

in vec2 coords;

uniform int frame;
uniform float dt;
uniform vec2 res; // window resolution
uniform vec2 mpos; // current mouse position
uniform vec2 rel; // relative mouse movement (in pixels)
uniform int mDown; // if 0 mouse is up, else, mouse is down

uniform sampler2D velTex; // velocity texture
uniform sampler2D tmpTex; // temporary texture
uniform sampler2D prsTex; // pressure texture
uniform sampler2D qntTex; // quantity texture

float delx = 1 / res.x;
float dely = 1 / res.y;

void jacobi(vec2 coords, out vec4 xNew, float alpha, float rbeta, sampler2D x, sampler2D b) {
    vec4 xL = texture(x, coords - vec2(delx, 0));
    vec4 xR = texture(x, coords + vec2(delx, 0));
    vec4 xB = texture(x, coords - vec2(0, dely));
    vec4 xT = texture(x, coords + vec2(0, dely));

    vec4 bC = texture(b, coords);

    xNew = (xL + xR + xB + xT + alpha * bC) * rbeta;
}

void main() {
    float alpha = -(delx*delx);
    float rbeta = 0.25;
    jacobi(coords, fragColor, alpha, rbeta, prsTex, tmpTex);

}