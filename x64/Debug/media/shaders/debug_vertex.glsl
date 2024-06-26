#version 420 core
layout (location = 0) in vec3 position;
layout (location = 1) in vec3 color;

out vec3 fragmentColor;

uniform mat4 view;
uniform mat4 projection;

void main() {
    gl_Position = projection * view * vec4(position, 1.0f);
    fragmentColor = color;
}
