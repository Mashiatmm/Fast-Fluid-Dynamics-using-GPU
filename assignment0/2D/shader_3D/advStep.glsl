#version 330

// smooth in vec3 fragNorm;	// Interpolated model-space normal

out vec4 fragColor;	// Final pixel color
in vec2 coords;

uniform float dt;
uniform vec2 res; // window resolution
uniform int frame;
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
    vec2 pos = coords - dt * (res.x / res.y) * texture(velTex, coords).xy;
    vec4 xL = texture(qntTex, pos - vec2(delx, 0));
    vec4 xR = texture(qntTex, pos + vec2(delx, 0));
    vec4 xB = texture(qntTex, pos - vec2(0, dely));
    vec4 xT = texture(qntTex, pos + vec2(0, dely));
    
    fragColor = mix(mix(xL, xR, 0.5), mix(xB, xT, 0.5), 0.5);

    vec4 force = vec4(0);
    if (mDown != 0) {
        vec2 orgPos = mpos / res; // original mouse position rescaled
        vec2 relMmt = rel / res; // relative mouse motion rescaled
        float dist = distance(coords, orgPos);
        float a = 0.12;
        float val = (a / (dist + a)) - 0.5;
        float frm = frame;
        if(dist < 0.15){
            force = vec4(val*cos(frm/100), val*sin(frm/100), val*sin(frm/300), 1); // the frm helps with changing colors
            force = abs(force);
            force *= 0.7;

        }
        
        
    }

    
    fragColor += force;
    // fragColor = vec4(1, 0, 0, 1);
}