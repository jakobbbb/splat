#version 430 core

layout (location = 0) in vec2 inPos;

uniform mat4 view;
uniform mat4 proj;

out vec4 PassColor;

struct Gaussian {
    vec3 pos;
    vec3 color;
    float opacity;
    // rotation matrix * scale matrix
    mat4 rot_scale;
};

layout(std430, binding = 0) buffer GaussianData {
    Gaussian gaussians[];
};

void main() {
    Gaussian gaussian = gaussians[gl_InstanceID];

    // covariance matrix
    mat4 sigma = gaussian.rot_scale * transpose(gaussian.rot_scale);

    // upper-left values of view matrix
    mat3 view3 = mat3(view);

    vec3 u = gaussian.pos;
    mat3 jacobian = mat3(
            1/u.z, 0,     -u.x/(u.z * u.z),
            0,     1/u.z, -u.y/(u.z * u.z),
            0,     0,     0
    );

    //gl_Position = proj * view * vec4(inPos, 1);
    gl_Position = vec4(inPos, 0, 0);

    PassColor = vec4(gaussian.color, gaussian.opacity);
}
