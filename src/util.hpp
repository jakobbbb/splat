#ifndef UTIL_HPP

#include <string>
#include <vector>
#include <GL/glew.h>

namespace splat::util {

std::string read_file(std::string const& path);

uint link_shaders(std::vector<uint> const& shaders);
uint load_shader(std::string const& path, GLenum type);

void cleanup();

static std::vector<uint> shaders{};

};  // namespace splat::util

#define UTIL_HPP
#endif  // UTIL_HPP
