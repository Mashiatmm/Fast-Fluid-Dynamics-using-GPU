#version 330

out vec4 fragColor;

in vec2 coords;

uniform int frame;
uniform float dt;
uniform vec2 res; // window resolution
uniform vec2 mpos; // current mouse position
uniform vec2 rel; // relative mouse movement (in pixels)
uniform int mDown; // if 0 mouse is up, else, mouse is down
uniform vec3 cubeCenter;
uniform vec3 cubeSize;

uniform sampler2D velTex; // velocity texture
uniform sampler2D tmpTex; // temporary texture
uniform sampler2D prsTex; // pressure texture
uniform sampler2D qntTex; // quantity texture

float delx = 1 / res.x;
float dely = 1 / res.y;


void divergence(vec2 coords, out vec4 div, sampler2D x) {
    vec4 xL = texture(x, coords - vec2(delx, 0));
    vec4 xR = texture(x, coords + vec2(delx, 0));
    vec4 xB = texture(x, coords - vec2(0, dely));
    vec4 xT = texture(x, coords + vec2(0, dely));

    fragColor = vec4((res.x / res.y) * 0.5 * ((xR.x - xL.x) + (xT.y - xB.y)));
}

void main() {
    // divergence(coords, fragColor, velTex);
    vec3 xL = texture(velTex, coords - vec2(delx, 0)).xyz;
    vec3 xR = texture(velTex, coords + vec2(delx, 0)).xyz;
    vec3 xB = texture(velTex, coords - vec2(0, dely)).xyz;
    vec3 xT = texture(velTex, coords + vec2(0, dely)).xyz;
    // fragColor = vec4(1, 0, 0, 1);
    vec3 result = vec3((1.0 * res.x / res.y) * 0.5 * ((xR.x - xL.x) + (xT.y - xB.y)));
    fragColor = vec4(result, 1);
}