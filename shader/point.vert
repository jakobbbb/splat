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

vec4 get_basis(mat2 sigma) {
    float a = sigma[0][0];
    float b = sigma[0][1];
    float c = sigma[1][0];
    float d = sigma[1][1];

    float tr = a + d;
    float det = (a * d) - (b * c);

    // eigenvalues
    float u = sqrt((tr * tr)/4-det);
    float e1 = tr/2 + u;
    float e2 = tr/2 - u;

    // eigenvectors
    vec2 v1 = vec2(b, e1 - a);
    vec2 v2 = vec2(b, e2 - a);

    // basis vectors
    vec2 b1 = e1 * v1;
    vec2 b2 = e2 * v2;

    return vec4(b1, b2);
}

void main() {
    Gaussian gaussian = gaussians[gl_InstanceID];

    // covariance matrix
    mat3 sigma = mat3(gaussian.rot_scale * transpose(gaussian.rot_scale));

    // upper-left values of view matrix
    mat3 view3 = mat3(view);

    vec3 u = gaussian.pos;
    mat3 jacobian = mat3(
            1/u.z, 0,     -u.x/(u.z * u.z),
            0,     1/u.z, -u.y/(u.z * u.z),
            0,     0,     0
    );

    mat3 p = view3 * jacobian;
    mat3 sigma_prime = transpose(p) * sigma * p;

    mat2 sigma2 = mat2(sigma_prime);  // 2d covariance

    vec4 bases = get_basis(sigma2);
    vec2 b1 = bases.xy;
    vec2 b2 = bases.zw;

    //gl_Position = proj * view * vec4(inPos, 1);
    gl_Position = vec4(inPos, -1, 1);

    PassColor = vec4(gaussian.color, gaussian.opacity);
}
