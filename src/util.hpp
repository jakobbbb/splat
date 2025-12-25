#ifndef UTIL_HPP

#include <GL/glew.h>
#include <glm/gtc/quaternion.hpp>
#include <string>
#include <vector>

namespace splat {

struct Gaussian {
    // X, Y, Z, W=1
    glm::vec4 pos;
    // R, G, B, A
    glm::vec4 color;
    // 3D Covariance, as mat4 for alignment
    glm::mat4 sigma;
};

}  // namespace splat

namespace splat::util {

std::string read_file(std::string const& path);

uint link_shaders(std::vector<uint> const& shaders);
uint load_shader(std::string const& path, GLenum type);

void cleanup();

static std::vector<uint> shaders{};

};  // namespace splat::util

#define UTIL_HPP
#endif  // UTIL_HPP
