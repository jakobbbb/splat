#version 430 core

layout (location = 0) in vec2 inPos;

uniform mat4 view;
uniform mat4 proj;
uniform vec2 viewport_size;

out vec4 PassColor;
out mat3 PassSigma;
out vec2 PassPosition;

struct Gaussian {
    vec4 pos;
    vec4 color;
    mat4 sigma;
};

layout(std430, binding = 0) buffer GaussianData {
    Gaussian gaussians[];
};

layout(std430, binding = 1) buffer Indices {
    int indices[];
};

void main() {
    Gaussian gaussian = gaussians[gl_InstanceID];

    vec4 p = proj * view * gaussian.pos;
    gl_Position = vec4(p.xyz / p.w, 1);
    gl_PointSize = 2;
    PassColor = vec4(gaussian.color.rgb, gaussian.color.a);

}
