#version 330 core

out vec4 finalColor;

uniform vec3 color; // Uniform for the line color

void main() {
    finalColor = vec4(color, 1.0);
}
