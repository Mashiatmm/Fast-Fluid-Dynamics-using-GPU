#version 330 

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

// Gradient
void gradient(vec2 coords, out vec4 uNew, sampler2D p, sampler2D w) {
    float pL = texture(p, coords - vec2(delx, 0)).x;
    float pR = texture(p, coords + vec2(delx, 0)).x;
    float pB = texture(p, coords - vec2(0, dely)).x;
    float pT = texture(p, coords + vec2(0, dely)).x;
    
    uNew = texture(w, coords);
    uNew.xy -= (res.x / res.y) * 0.5 * vec2(pR - pL, pT - pB);
}

void main() {
    gradient(coords, fragColor, prsTex, velTex);
}