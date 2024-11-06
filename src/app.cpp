#include "app.hpp"
#include "util.hpp"

#include <cassert>
#include <iostream>
#include <vector>

namespace splat {

App::App() {
    init_window();
    load_data();
    load_shaders();
}

void App::init_window() {
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    win = glfwCreateWindow(WIDTH, HEIGHT, "Hello", nullptr, nullptr);
    assert(win != nullptr);
    glfwMakeContextCurrent(win);

    auto resize_callback = [](GLFWwindow* window, int width, int height) {
        glViewport(0, 0, width, height);
    };
    glfwSetFramebufferSizeCallback(win, resize_callback);

    glewInit();
}

void App::load_data() {
    std::vector<float> data = {0.0f, 0.0f};
    glGenBuffers(1, &vertex_buffer);
    glBindBuffer(GL_ARRAY_BUFFER, vertex_buffer);
    glBufferData(GL_ARRAY_BUFFER, data.size(), data.data(), GL_STATIC_DRAW);
}

void App::load_shaders() {
    auto vert = util::load_shader("../shader/point.vert", GL_VERTEX_SHADER);
    auto frag = util::load_shader("../shader/point.frag", GL_FRAGMENT_SHADER);
    point_shader = util::link_shaders({vert, frag});
}

void App::run() {
    while (!glfwWindowShouldClose(win)) {
        glfwSwapBuffers(win);
        glfwPollEvents();
        process_inputs();
        draw();
        ++frame;
    }
}

void App::process_inputs() {
    if (glfwGetKey(win, GLFW_KEY_Q) == GLFW_PRESS) {
        glfwSetWindowShouldClose(win, GLFW_TRUE);
    }
}

void App::draw() {
    glClearColor(0.0f, 0.0f, 0.4f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glEnable(GL_PROGRAM_POINT_SIZE);
    glUseProgram(point_shader);

    GLuint vao;
    glGenVertexArrays(1, &vao);
    glBindVertexArray(vao);

    glBindVertexBuffer(0, vertex_buffer, 0, 0);
    glDrawArrays(GL_POINTS, 0, 1);
}

}  // namespace splat

int main(int argc, char** argv) {
    auto app = splat::App();
    app.run();
}
