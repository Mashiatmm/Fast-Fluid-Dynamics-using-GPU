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


#define DENSITY 1
#define VISCOSITY 1
#define FORCEMULT 0.3



void main() {
    vec4 force = vec4(0);
   
    if (mDown != 0) {
        vec2 orgPos = mpos / res; // original mouse position rescaled
        vec2 relMmt = rel / res; // relative mouse motion rescaled

        vec2 F = relMmt * FORCEMULT;

        force = vec4(1*F/distance(coords, orgPos), 0, 0);
    }

    // // Apply cube boundary enforcement
    // vec3 Center = cubeCenter * 0.5 + 0.5;
    // vec3 Size = cubeSize * 0.5;
    // vec3 minBound = (Center - Size);
    // vec3 maxBound = (Center + Size);


    // // Check if the position is inside the cube bounds
    // if (coords.x >= minBound.x && coords.x <= maxBound.x &&
    //     coords.y >= minBound.y && coords.y <= maxBound.y) {
    //     force = vec4(0, 0, 0, 0); // Enforce no-slip condition (velocity = 0)
    //     // fragColor = vec4(1, 0, 0, 0);
    // }

    fragColor = texture(velTex, coords) + force;

}