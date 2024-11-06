#include "app.hpp"
#include "external/happly.h"
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
    const char* ply_path =
            "/mnt/wd/home/jakob/archive/splat_models/train/point_cloud/iteration_7000/"
            "point_cloud.ply";
    happly::PLYData plyIn(ply_path);
    std::vector<std::array<double, 3>> vPos = plyIn.getVertexPositions();
    std::cout << vPos.size() << "\n";
    num_gaussians = vPos.size();

    glGenVertexArrays(1, &vao);
    glBindVertexArray(vao);

    std::vector<float> data = {};
    data.reserve(vPos.size());

    for (const auto& p : vPos) {
        data.push_back((float)p[0]);
        data.push_back((float)p[1]);
        data.push_back((float)p[2]);
    }

    glGenBuffers(1, &vertex_buffer);
    glBindBuffer(GL_ARRAY_BUFFER, vertex_buffer);
    glBufferData(GL_ARRAY_BUFFER, data.size() * sizeof(float), data.data(), GL_STATIC_DRAW);
    glVertexAttribPointer(0,                  // location = 0
                          3,                  // attribute size
                          GL_FLOAT,           // attribute type
                          GL_FALSE,           // don't normalize
                          3 * sizeof(float),  // stride
                          (void*)0            // offset
    );
    glEnableVertexAttribArray(0);

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

    if (glfwGetKey(win, GLFW_KEY_W) == GLFW_PRESS) {
        cam.translate({0, 0, 1});
    }
    if (glfwGetKey(win, GLFW_KEY_S) == GLFW_PRESS) {
        cam.translate({0, 0, -1});
    }
    if (glfwGetKey(win, GLFW_KEY_A) == GLFW_PRESS) {
        cam.translate({1, 0, 0});
    }
    if (glfwGetKey(win, GLFW_KEY_D) == GLFW_PRESS) {
        cam.translate({-1, 0, 0});
    }
    if (glfwGetKey(win, GLFW_KEY_SPACE) == GLFW_PRESS) {
        cam.translate({0, -1, 0});
    }
    if (glfwGetKey(win, GLFW_KEY_LEFT_CONTROL) == GLFW_PRESS) {
        cam.translate({0, 1, 0});
    }
}

void App::draw() {
    glClearColor(0.0f, 0.0f, 0.4f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glEnable(GL_PROGRAM_POINT_SIZE);
    glUseProgram(point_shader);

    auto proj = cam.get_proj();
    auto view = cam.get_view();
    GLint loc_proj = glGetUniformLocation(point_shader, "proj");
    GLint loc_view = glGetUniformLocation(point_shader, "view");
    glUniformMatrix4fv(loc_proj, 1, GL_FALSE, &proj[0][0]);
    glUniformMatrix4fv(loc_view, 1, GL_FALSE, &view[0][0]);

    glBindVertexArray(vao);

    glDrawArrays(GL_POINTS, 0, num_gaussians);
}

}  // namespace splat

int main(int argc, char** argv) {
    auto app = splat::App();
    app.run();
}
