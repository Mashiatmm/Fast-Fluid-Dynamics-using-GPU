#version 330

out vec4 fragColor;

in vec3 coords; // Coordinates of the current fragment in 3D space

uniform sampler3D velTex; // Velocity texture
uniform vec3 cubeCenter;  // Center of the cube
uniform vec3 cubeSize;    // Half-extents of the cube


void main() {
    // Sample velocity from texture
    vec3 velocity = texture(velTex, coords).xyz;

    // Apply cube boundary enforcement
    vec3 Center = cubeCenter + 0.5;
    vec3 Size = cubeSize * 0.5;
    vec3 minBound = (Center - Size) ;
    vec3 maxBound = (Center + Size) ;


    // Check if the position is inside the cube bounds
    if (coords.x >= minBound.x && coords.x <= maxBound.x &&
        coords.y >= minBound.y && coords.y <= maxBound.y) {
        velocity = vec3(0.0);  // Enforce no-slip condition (velocity = 0)
    }
    // Output updated velocity for debugging or visualization
    fragColor = vec4(velocity, 1.0);
}
