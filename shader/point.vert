#version 330 core

layout (location = 0) in vec3 inPos;
layout (location = 1) in vec3 inColor;

uniform mat4 view;
uniform mat4 proj;

out vec3 PassColor;

void main() {
    gl_Position = proj * view * vec4(inPos, 1);
    //gl_Position.x -= 0.02 * gl_InstanceID;
    gl_PointSize = 8.0;

    PassColor = inColor;
}
