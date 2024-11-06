#include "util.hpp"

#include <GL/glew.h>
#include <fstream>
#include <iostream>
#include <sstream>
#include <string>

namespace splat {

std::string util::read_file(std::string const& path_full) {
    std::ifstream file{path_full};
    if (!file.is_open()) {
        throw std::runtime_error("Failed to open file " + path_full + "!");
    }
    std::stringstream buf;
    buf << file.rdbuf();
    return buf.str();
}

uint util::load_shader(std::string const& path, GLenum type) {
    std::string shader_source_str = util::read_file(path);
    const char* shader_source = shader_source_str.c_str();
    uint shader = glCreateShader(type);
    glShaderSource(shader, 1, &shader_source, NULL);
    glCompileShader(shader);

    int success;
    glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
    if (!success) {
        char info_buf[512];
        glGetShaderInfoLog(shader, 512, NULL, info_buf);
        std::cout << "Shader compilation of " << path << " failed:\n"
                  << info_buf << '\n';
    }

    return shader;
}

uint util::link_shaders(std::vector<uint> const& shaders) {
    uint prog = glCreateProgram();
    for (auto const& shader : shaders) {
        glAttachShader(prog, shader);
    }
    glLinkProgram(prog);

    int success;
    glGetProgramiv(prog, GL_LINK_STATUS, &success);
    if (!success) {
        char info_buf[512];
        glGetProgramInfoLog(prog, 512, NULL, info_buf);
        std::cout << "Shader linking failed:\n" << info_buf << '\n';
    }
    return prog;
}

void util::cleanup() {
    for (auto const& shader : util::shaders) {
        glDeleteShader(shader);
    }
}

}  // namespace splat
