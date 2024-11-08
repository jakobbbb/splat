#include "app.hpp"
#include "external/happly.h"
#include "util.hpp"

#include <cassert>
#include <iostream>
#include <vector>

namespace splat {

App::App(char* ply_path) {
    init_window();
    load_data(ply_path);
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

void App::load_data(char* ply_path) {
    happly::PLYData ply(ply_path);
    std::vector<std::array<double, 3>> v_pos = ply.getVertexPositions();
    num_gaussians = v_pos.size();

    std::vector<std::string> properties = {
            "x",
            "y",
            "z",
            "f_dc_0",
            "f_dc_1",
            "f_dc_2",
            "opacity",
            "scale_0",
            "scale_1",
            "scale_2",
            "rot_0",
            "rot_1",
            "rot_2",
            "rot_3",
    };
    std::vector<size_t> property_sizes = {
            3,  // x, y, z
            4,  // color + opacity
            3,  // scale
            4,  // rotation
    };

    std::vector<std::vector<float>> values{};

    std::vector<float> data = {};
    data.reserve(num_gaussians * properties.size());

    for (size_t i = 0; i < properties.size(); ++i) {
        auto property = properties[i];
        values.emplace_back(ply.getElement("vertex").getProperty<float>(property));
    }

    for (size_t i_gaussian = 0; i_gaussian < num_gaussians; ++i_gaussian) {
        for (size_t i_property = 0; i_property < properties.size(); ++i_property) {
            float value = values[i_property][i_gaussian];

            if (properties[i_property][0] == 'f') {
                // https://en.wikipedia.org/wiki/Table_of_spherical_harmonics#%E2%84%93_=_0
                const float SH_0 = 0.28209479177387814f;
                value = 0.5f + SH_0 * value;
            }

            data.push_back(value);
        }
    }

    // load into ssbo
    glGenBuffers(1, &ssbo_buf);
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, ssbo_buf);
    glBufferData(
            GL_SHADER_STORAGE_BUFFER, data.size() * sizeof(float), data.data(), GL_DYNAMIC_COPY);
    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, ssbo_buf);

    std::vector<float> verts = {-1, -1, 1, -1, 1, 1, -1, 1};
    glGenBuffers(1, &vertex_buffer);
    glBindBuffer(GL_ARRAY_BUFFER, vertex_buffer);
    glBufferData(GL_ARRAY_BUFFER, verts.size() * sizeof(float), verts.data(), GL_STATIC_DRAW);

    glGenVertexArrays(1, &vao);
    glBindVertexArray(vao);
    glVertexAttribPointer(0,                  // location
                          2,                  // attribute size
                          GL_FLOAT,           // attribute type
                          GL_FALSE,           // don't normalize
                          2 * sizeof(float),  // stride
                          0                   // offset
    );
    glEnableVertexAttribArray(0);

    const bool load_into_vbo = false;
    if (load_into_vbo) {
        glGenVertexArrays(1, &vao);
        glBindVertexArray(vao);

        glGenBuffers(1, &vertex_buffer);
        glBindBuffer(GL_ARRAY_BUFFER, vertex_buffer);
        glBufferData(GL_ARRAY_BUFFER, data.size() * sizeof(float), data.data(), GL_STATIC_DRAW);

        size_t offset = 0;
        for (size_t i = 0; i < property_sizes.size(); ++i) {
            glVertexAttribPointer(i,                                  // location
                                  property_sizes[i],                  // attribute size
                                  GL_FLOAT,                           // attribute type
                                  GL_FALSE,                           // don't normalize
                                  properties.size() * sizeof(float),  // stride
                                  (void*)(offset * sizeof(float))     // offset
            );
            glEnableVertexAttribArray(i);
            offset += property_sizes[i];
        }
    }
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

    if (glfwGetMouseButton(win, GLFW_MOUSE_BUTTON_RIGHT) == GLFW_PRESS) {
        double x, y;
        glfwGetCursorPos(win, &x, &y);
        cam.update_rot(x, y);
    }
    if (glfwGetMouseButton(win, GLFW_MOUSE_BUTTON_RIGHT) == GLFW_RELEASE) {
        cam.reset_mouse();
    }
}

void App::draw() {
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glEnable(GL_PROGRAM_POINT_SIZE);
    glEnable(GL_BLEND);
    glUseProgram(point_shader);

    int w, h;
    glfwGetFramebufferSize(win, &w, &h);
    cam.update_res(w, h);

    auto proj = cam.get_proj();
    auto view = cam.get_view();
    GLint loc_proj = glGetUniformLocation(point_shader, "proj");
    GLint loc_view = glGetUniformLocation(point_shader, "view");
    glUniformMatrix4fv(loc_proj, 1, GL_FALSE, &proj[0][0]);
    glUniformMatrix4fv(loc_view, 1, GL_FALSE, &view[0][0]);

    glBindVertexArray(vao);
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, ssbo_buf);

    glDrawArraysInstanced(GL_TRIANGLE_FAN, 0, 4, num_gaussians);
}

}  // namespace splat

int main(int argc, char** argv) {
    if (argc < 2) {
        std::cout << "usage: " << argv[0] << " <point_cloud.ply>\n";
        return 1;
    }
    char* ply_path = argv[1];
    auto app = splat::App(ply_path);
    app.run();
}
