#version 430 core

in vec4 PassColor;
in mat3 PassSigma;
in vec2 PassPosition;

out vec4 FragColor;

void main() {
    FragColor = vec4(1, 1, 0, 1);
    FragColor = PassColor;
    // return;

    int f = 4;
    float A = -dot(f * PassPosition, f * PassPosition);
    if (A < -4.0) discard;
    float B = exp(A) * PassColor.a;
    FragColor = vec4(B * PassColor.rgb, B);
}
