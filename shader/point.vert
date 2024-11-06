#version 330 core

layout (location = 0) in vec3 inPos;

void main() {
    gl_Position = vec4(inPos, 1);
    gl_PointSize = 80.0;
}
