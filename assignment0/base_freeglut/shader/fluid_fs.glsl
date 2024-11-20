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


void main() {
    vec4 v = texture(velTex, coords);
    vec4 t = texture(tmpTex, coords);
    vec4 p = texture(prsTex, coords);
    vec4 q = texture(qntTex, coords);
    fragColor = vec4(q.xyz, 1);
    //fragColor = vec4(1, 0, 0, 1);
}