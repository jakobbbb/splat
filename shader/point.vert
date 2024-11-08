#version 430 core

layout (location = 0) in vec2 inPos;

uniform mat4 view;
uniform mat4 proj;

out vec4 PassColor;

struct Gaussian {
    vec3 pos;
    vec3 color;
    float opacity;
    vec3 scale;
    vec4 rot;
};

layout(std430, binding = 0) buffer GaussianData {
    Gaussian gaussians[];
};

void main() {
    Gaussian gaussian = gaussians[gl_InstanceID];

    //gl_Position = proj * view * vec4(inPos, 1);
    gl_Position = vec4(inPos, 0, 0);

    PassColor = vec4(gaussian.color, gaussian.opacity);
}
