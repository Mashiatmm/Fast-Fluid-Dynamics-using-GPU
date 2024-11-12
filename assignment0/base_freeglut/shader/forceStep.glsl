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


#define DENSITY 1
#define VISCOSITY 1
#define FORCEMULT 0.3



void main() {
    vec4 force = vec4(0);
    if (mDown != 0) {
         vec2 orgPos = mpos / res; // original mouse position rescaled
        vec2 relMmt = rel / res; // relative mouse motion rescaled

        vec2 F = relMmt * FORCEMULT;

        force = vec4(F*1/distance(coords, orgPos), 0, 0);
    }
    fragColor = texture(velTex, coords) + force;
}