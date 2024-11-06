#version 330 core

layout (location = 0) in vec3 inPos;

void main() {
    gl_Position = vec4(inPos, 1);
    gl_Position.x -= 0.02 * gl_InstanceID;
    gl_PointSize = 8.0;
}
