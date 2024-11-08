#version 430 core

in vec4 PassColor;
in mat3 PassSigma;

out vec4 FragColor;

void main() {
    FragColor = PassColor;
}
