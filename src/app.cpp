#include "app.hpp"
#include "external/happly.h"
#include "util.hpp"

#define GLM_ENABLE_EXPERIMENTAL  // waow

#include <cassert>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/quaternion.hpp>
#include <glm/gtx/string_cast.hpp>
#include <iostream>
#include <map>
#include <vector>

namespace splat {

App::App(char* ply_path) {
    init_window();
    load_data(ply_path);
    load_shaders();
    std::cout << "ok\n";
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
    std::cout << "Reading ply...\n";
    happly::PLYData ply(ply_path);
    std::vector<std::array<double, 3>> v_pos = ply.getVertexPositions();
    num_gaussians = v_pos.size();
    std::cout << "Got " << num_gaussians << " gaussians\n";

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

    std::map<std::string, std::vector<float>> values{};

    std::vector<Gaussian> data = {};
    data.reserve(num_gaussians * sizeof(Gaussian));

    for (size_t i = 0; i < properties.size(); ++i) {
        auto property = properties[i];
        values[property] = (ply.getElement("vertex").getProperty<float>(property));
    }

    std::cout << "Extracting gaussians...\n";
    for (size_t i = 0; i < num_gaussians; ++i) {
        Gaussian g{};

        g.pos = {
                values["x"][i],
                values["y"][i],
                values["z"][i],
                1.0f,
        };

        g.color = {
                values["f_dc_0"][i],
                values["f_dc_1"][i],
                values["f_dc_2"][i],
                values["opacity"][i],
        };
        // extract base color from spherical harmonics
        const float SH_0 = 0.28209479177387814f;
        g.color = 0.5f + SH_0 * g.color;


        glm::vec3 scale = {
                values["scale_0"][i],
                values["scale_1"][i],
                values["scale_2"][i],
        };
        glm::mat4 scale_mat = glm::scale(glm::mat4(1.0f), scale);

        glm::quat rot = {
                values["rot_0"][i],
                values["rot_1"][i],
                values["rot_2"][i],
                values["rot_3"][i],
        };
        glm::mat4 rot_mat = glm::mat4(glm::mat3(rot));

        auto rot_scale = rot_mat * scale_mat;
        g.sigma = glm::mat4(rot_scale * glm::transpose(rot_scale));

        data.push_back(g);
    }

    std::cout << "Loading ssbo...\n";
    // load into ssbo
    glGenBuffers(1, &ssbo_buf);
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, ssbo_buf);
    glBufferData(
            GL_SHADER_STORAGE_BUFFER, data.size() * sizeof(Gaussian), data.data(), GL_DYNAMIC_COPY);
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
}

void App::load_shaders() {
    auto vert = util::load_shader("../shader/gaussian.vert", GL_VERTEX_SHADER);
    auto frag = util::load_shader("../shader/gaussian.frag", GL_FRAGMENT_SHADER);
    gaussian_shader = util::link_shaders({vert, frag});

    auto vert_p = util::load_shader("../shader/point.vert", GL_VERTEX_SHADER);
    auto frag_p = util::load_shader("../shader/point.frag", GL_FRAGMENT_SHADER);
    point_shader = util::link_shaders({vert_p, frag_p});

    shader = point_shader;
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
        cam.translate(cam.forward());
    }
    if (glfwGetKey(win, GLFW_KEY_S) == GLFW_PRESS) {
        cam.translate(-cam.forward());
    }
    if (glfwGetKey(win, GLFW_KEY_A) == GLFW_PRESS) {
        cam.translate(-cam.right());
    }
    if (glfwGetKey(win, GLFW_KEY_D) == GLFW_PRESS) {
        cam.translate(cam.right());
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

    if (glfwGetKey(win, GLFW_KEY_G) == GLFW_PRESS) {
        shader = gaussian_shader;
    }
    if (glfwGetKey(win, GLFW_KEY_P) == GLFW_PRESS) {
        shader = point_shader;
    }
}

void App::draw() {
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glEnable(GL_VERTEX_PROGRAM_POINT_SIZE);
    glDisable(GL_DEPTH_TEST);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    glUseProgram(shader);

    int w, h;
    glfwGetFramebufferSize(win, &w, &h);
    cam.update_res(w, h);

    auto proj = cam.get_proj();
    auto view = cam.get_view();
    float viewport_size[] = {(float)w, (float)h};
    GLint loc_proj = glGetUniformLocation(shader, "proj");
    GLint loc_view = glGetUniformLocation(shader, "view");
    GLint loc_viewport_size = glGetUniformLocation(shader, "viewport_size");
    glUniformMatrix4fv(loc_proj, 1, GL_FALSE, &proj[0][0]);
    glUniformMatrix4fv(loc_view, 1, GL_FALSE, &view[0][0]);
    glUniform2fv(loc_viewport_size, 1, viewport_size);

    glBindVertexArray(vao);
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, ssbo_buf);

    if (shader == gaussian_shader) {
        glDrawArraysInstanced(GL_TRIANGLE_FAN, 0, 4, num_gaussians);
    } else {
        glDrawArraysInstanced(GL_POINTS, 0, 1, num_gaussians);
    }

    glm::vec2 focal{
        proj[0][0] * w * 0.5,
        proj[1][1] * h * 0.5
    };
    if (frame % 100 == 0) {
        std::cout << "view: " << glm::to_string(view) << "\n";
        std::cout << "proj: " << glm::to_string(proj) << "\n";
        std::cout << "cam@: " << glm::to_string(cam.get_pos()) << "\n";
        std::cout << "foca: " << glm::to_string(focal) << "\n";
    }
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
