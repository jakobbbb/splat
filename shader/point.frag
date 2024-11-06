#version 330 core

in vec3 PassColor;

out vec4 FragColor;

void main() {
    FragColor = vec4(PassColor, 1.0);
}
