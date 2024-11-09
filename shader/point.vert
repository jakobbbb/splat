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

vec4 get_basis(mat2 sigma) {
    float a = sigma[0][0];
    float b = sigma[0][1];
    float c = sigma[1][0];
    float d = sigma[1][1];

    float tr = a + d;
    float det = a * d - b * c;

    // eigenvalues
    float s = sqrt((tr * tr) - (4 * det));
    float lambda1 = 0.5 * (tr + s);
    float lambda2 = 0.5 * (tr - s);

    // eigenvectors
    const float epsilon = 0.00001;

    vec2 e1 = vec2(1, 0);
    if (abs(c) > epsilon) {
        e1 = vec2(lambda1 - d, c);
    } else if (abs(b) > epsilon) {
        e1 = vec2(b, lambda1 - a);
    }
    e1 = normalize(e1);

    vec2 e2 = vec2(e1.y, -e1.x);

    // basis vectors
    vec2 b1 = sqrt(2 * lambda1) * e1;
    vec2 b2 = sqrt(2 * lambda2) * e2;

    return vec4(b1, b2);
}

void main() {
    Gaussian gaussian = gaussians[gl_InstanceID];

    // upper-left values of view matrix
    mat3 view3 = mat3(view);

    vec3 u = view3 * gaussian.pos.xyz;

    vec4 pos2d = proj * view * gaussian.pos;

    mat3 jacobian = mat3(
            1/u.z, 0,     -u.x/(u.z * u.z),
            0,     1/u.z, -u.y/(u.z * u.z),
            0,     0,     0
    );

    mat3 t = view3 * jacobian;
    mat3 sigma_prime = transpose(t) * mat3(gaussian.sigma) * t;

    mat2 sigma2 = mat2(sigma_prime);  // 2d covariance

    vec4 bases = get_basis(sigma2);
    vec2 b1 = bases.xy;
    vec2 b2 = bases.zw;

    //b1 = 512 * vec2(1, 1);
    //b2 = 512 * vec2(-1, 1);

    //gl_Position = proj * view * vec4(inPos, -1, 1);
    vec2 center = pos2d.xy / pos2d.w;
    //center = vec2(0, 0);
    gl_Position = vec4(center
            + (inPos.x * b1 / viewport_size)
            + (inPos.y * b2 / viewport_size),
            -1, 1);

    int i = int(2 * inPos.x / 2);
    int j = int(2 * inPos.y / 2);

    PassColor = vec4(gaussian.color.rgb, gaussian.color.a);

    PassPosition = inPos;

    //PassColor = vec4(b2.x, b2.y, 0, 1);
    //gl_Position = vec4(inPos, -1, 1);
    //PassColor = vec4(sigma_prime[2].xyz, 1);
}
