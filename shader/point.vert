#version 330 core

layout (location = 0) in vec3 inPos;
layout (location = 1) in vec4 inColor;

uniform mat4 view;
uniform mat4 proj;

out vec4 PassColor;

void main() {
    gl_Position = proj * view * vec4(inPos, 1);
    //gl_Position.x -= 0.02 * gl_InstanceID;
    gl_PointSize = 1.6;

    PassColor = inColor;
}
