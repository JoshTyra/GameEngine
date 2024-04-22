#version 430 core

in float firstBoneInfluence; // Received from the vertex shader
out vec4 FragColor;

void main() {
    // Color the fragment based on the first bone's influence
    // This creates a gradient from red (no influence) to green (full influence)
    FragColor = vec4(1.0 - firstBoneInfluence, firstBoneInfluence, 0.0, 1.0);
}
