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

uniform float scale;
uniform sampler2D field;
uniform vec2 rdv;
uniform vec2 offset;

void main() {
    vec2 value = texture2D(field, coords + (offset * rdv)).xy;
	value = scale * value;
	fragColor = vec4(value, 0.0, 1.0);

    // // Determine if the fragment is on the boundary
    // // float scale = -1;
    // float threshold = 1.0 / res.x; // Adjust for texture resolution
    // bool isLeft   = coords.x <= threshold;
    // bool isRight  = coords.x >= (res.x - threshold);
    // bool isBottom = coords.y <= threshold;
    // bool isTop    = coords.y >= (res.y - threshold);

    // // Initialize boundary value
    // vec4 bv = vec4(0.0);

    // // Apply boundary conditions
    // if (isLeft) {
    //     bv = scale * texture(velTex, coords + vec2(1.0, 0.0));
    // } else if (isRight) {
    //     bv = scale * texture(velTex, coords - vec2(1.0, 0.0));
    // } else if (isBottom) {
    //     bv = scale * texture(velTex, coords + vec2(0.0, 1.0));
    // } else if (isTop) {
    //     bv = scale * texture(velTex, coords - vec2(0.0, 1.0));
    // } else {
    //     // For non-boundary fragments, fetch the original value
    //     bv = texture(velTex, coords);
    // }

    // fragColor = bv;
}
