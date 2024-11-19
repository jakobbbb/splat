#include "app.hpp"
#include <cmath>
#include <cstdint>
#include "external/happly.h"
#include "external/miniply/miniply.h"
#include "util.hpp"

#define GLM_ENABLE_EXPERIMENTAL  // waow

#include <algorithm>
#include <cassert>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/quaternion.hpp>
#include <glm/gtx/string_cast.hpp>
#include <iostream>
#include <map>
#include <vector>

#include <chrono>
#include <ctime>

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
    glfwSwapInterval(0);  // disable vsync

    glewInit();
}

/**
 * Load data from .ply file into an SSBO that is an array of `Gaussian` structs.
 * Disregards view-dependent spherical harmonic colors.
 */
void App::load_data(char* ply_path) {

    std::cout << "Reading ply...\n";
    const std::vector<std::string> properties = {
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

    // check if file exists
    miniply::PLYReader reader(ply_path);
    if (!reader.valid()) {
        std::cerr << "Failed to open " << ply_path << std::endl;
    }

    // check we have vertex elements
    miniply::PLYElement* gsSplatEl = reader.get_element(reader.find_element("vertex"));
    if (gsSplatEl == nullptr) {
        std::cerr << "no vertex element could be found" << std::endl;
    }

    // could check if loading succeeds
    //reader.load_element();
    uint32_t num_gaussians = reader.num_rows();
    std::cout << "Got " << num_gaussians << " gaussians (" << num_gaussians * sizeof(Gaussian) / 1e6 << "MB)\n";

    // i guess
    // uint32_t splatPropertyIdx[gsSplatEl->properties.size()];
    // gsSplatEl->convert_list_to_fixed_size(gsSplatEl->properties.size()-1 , 3, splatPropertyIdx);


    // getting all the data we neeed for the splats
    uint32_t gaussSplatIdx[properties.size()];
    for (int i=0; i<properties.size(); ++i) {
        gaussSplatIdx[i] = gsSplatEl->find_property(properties[i].c_str());
    }
    

    



    auto start_time = std::chrono::system_clock::now();
    
    // happly::PLYData ply(ply_path);
    // std::vector<std::array<double, 3>> v_pos = ply.getVertexPositions();
    // num_gaussians = v_pos.size();
    // std::cout << "Got " << num_gaussians << " gaussians (" << num_gaussians * sizeof(Gaussian) / 1e6 << "MB)\n";


    // std::map<std::string, std::vector<float>> values{};

    data.reserve(num_gaussians);

    // for (size_t i = 0; i < properties.size(); ++i) {
    //     auto property = properties[i];
    //     values[property] = (ply.getElement("vertex").getProperty<float>(property));
    // }

    // bounds = {
    //         {values["x"][0], values["y"][0], values["z"][0]},
    //         {values["x"][0], values["y"][0], values["z"][0]},
    // };

    std::cout << "Extracting gaussians...\n";
    const float SH_0 = 0.28209479177387814f;
    // for (size_t i = 0; i < num_gaussians; ++i) {
    //     Gaussian g{};

    //     g.pos = {
    //             values["x"][i],
    //             values["y"][i],
    //             values["z"][i],
    //             1.0f,
    //     };

    //     g.color = {
    //             values["f_dc_0"][i],
    //             values["f_dc_1"][i],
    //             values["f_dc_2"][i],
    //             0.0f,
    //     };
    //     // extract base color from spherical harmonics
        
    //     g.color = 0.5f + SH_0 * g.color;
    //     g.color.a = 1.0f / (1.0f + exp(-(values["opacity"][i])));

    //     glm::vec3 scale = {
    //             glm::exp(values["scale_0"][i]),
    //             glm::exp(values["scale_1"][i]),
    //             glm::exp(values["scale_2"][i]),
    //     };
    //     glm::mat4 scale_mat = glm::scale(glm::mat4(1.0f), scale);

    //     glm::quat rot = {
    //             values["rot_0"][i],
    //             values["rot_1"][i],
    //             values["rot_2"][i],
    //             values["rot_3"][i],
    //     };
    //     glm::mat4 rot_mat = glm::mat4(glm::mat3(rot));

    //     auto rot_scale = rot_mat * scale_mat;
    //     g.sigma = rot_scale * glm::transpose(rot_scale);

    //     //data.push_back(g);

    //     // update bounds
    //     for (int i = 0; i < 2; ++i) {
    //         bounds.first.x = glm::min(bounds.first.x, g.pos.x);
    //         bounds.first.y = glm::min(bounds.first.y, g.pos.y);
    //         bounds.first.z = glm::min(bounds.first.z, g.pos.z);
    //         bounds.second.x = glm::max(bounds.second.x, g.pos.x);
    //         bounds.second.y = glm::max(bounds.second.y, g.pos.y);
    //         bounds.second.z = glm::max(bounds.second.z, g.pos.z);
    //     }
    // }
    auto end_time = std::chrono::system_clock::now();
    std::chrono::duration<double> duration_in_s = end_time - start_time;
    std::cout << "Loading object took " << duration_in_s.count() << "s" << std::endl;
    std::cout << "Bounds:  min=" << glm::to_string(bounds.first)
              << ", max=" << glm::to_string(bounds.second) << "\n";

    // while (reader.has_element()) {
    //   if (!reader.load_element() || !reader.find_pos(gaussSplatIdx)) {
    //     break;
    //   }
    
    start_time = std::chrono::system_clock::now();
    reader.load_element();
    float* line = new float[properties.size() * num_gaussians];
    reader.extract_properties(gaussSplatIdx, properties.size(), miniply::PLYPropertyType::Float, line);
    for (size_t i = 0; i < num_gaussians; ++i) {
      Gaussian g{};
        int offset = i * properties.size();
        g.pos = {
                line[offset + 0],
                line[offset + 1],
                line[offset + 2],
                1.0f,
        };

        g.color = {
                line[offset + 3],
                line[offset + 4],
                line[offset + 5],
                0.0f,
        };
        // extract base color from spherical harmonics
        
        g.color = 0.5f + SH_0 * g.color;
        g.color.a = 1.0f / (1.0f + exp(-(line[offset + 6])));

        glm::vec3 scale = {
                glm::exp(line[offset + 7]),
                glm::exp(line[offset + 8]),
                glm::exp(line[offset + 9]),
        };
        glm::mat4 scale_mat = glm::scale(glm::mat4(1.0f), scale);

        glm::quat rot = {
                line[offset + 10],
                line[offset + 11],
                line[offset + 12],
                line[offset + 13],
        };
        glm::mat4 rot_mat = glm::mat4(glm::mat3(rot));

        auto rot_scale = rot_mat * scale_mat;
        g.sigma = rot_scale * glm::transpose(rot_scale);

        data.push_back(g);

        // update bounds
        for (int i = 0; i < 2; ++i) {
            bounds.first.x = glm::min(bounds.first.x, g.pos.x);
            bounds.first.y = glm::min(bounds.first.y, g.pos.y);
            bounds.first.z = glm::min(bounds.first.z, g.pos.z);
            bounds.second.x = glm::max(bounds.second.x, g.pos.x);
            bounds.second.y = glm::max(bounds.second.y, g.pos.y);
            bounds.second.z = glm::max(bounds.second.z, g.pos.z);
        }
      // reader.next_element();
    }
    end_time = std::chrono::system_clock::now();
    duration_in_s = end_time - start_time;
    std::cout << "Loading object took " << duration_in_s.count() << "s" << std::endl;


    std::cout << "Loading ssbo...\n";

    // Create and fill Gaussian SSBO
    glGenBuffers(1, &gauss_ssbo);
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, gauss_ssbo);
    glBufferData(
            GL_SHADER_STORAGE_BUFFER, data.size() * sizeof(Gaussian), data.data(), GL_DYNAMIC_COPY);
    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, gauss_ssbo);

    // Create and fill index SSBO.  After sorting, this will contain indices into the Gaussian SSBO,
    // in order.
    indices.reserve(num_gaussians);
    for (int i = 0; i < num_gaussians; ++i) {
        indices.push_back(i);
    }
    glGenBuffers(1, &index_ssbo);
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, index_ssbo);
    glBufferData(GL_SHADER_STORAGE_BUFFER,
                 indices.size() * sizeof(int),
                 indices.data(),
                 GL_DYNAMIC_COPY);
    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 1, index_ssbo);

    // Create vertex buffer with a single screen-space quad.
    std::vector<float> verts = {-2, -2, 2, -2, 2, 2, -2, 2};
    glGenBuffers(1, &vertex_buffer);
    glBindBuffer(GL_ARRAY_BUFFER, vertex_buffer);
    glBufferData(GL_ARRAY_BUFFER, verts.size() * sizeof(float), verts.data(), GL_STATIC_DRAW);

    glGenVertexArrays(1, &vao);
    glBindVertexArray(vao);
    glVertexAttribPointer(0,                  // location
                          2,                  // attribute size (2D position on screen)
                          GL_FLOAT,           // attribute type
                          GL_FALSE,           // don't normalize
                          2 * sizeof(float),  // stride
                          0                   // offset
    );
    glEnableVertexAttribArray(0);
}

/*
 * Sort Gaussians based on distance to camera using counting sort.  Because the key for counting
 * sort needs to be an integer, we cannot guarantee exact sorting.
 */
void App::csort() {
    glm::vec4 cam_pos = glm::vec4(cam.get_pos(), 1);
    const size_t n_buckets = 65534;

    std::vector<size_t> count(n_buckets + 1, 0);

    std::vector<size_t> distances{};
    distances.reserve(num_gaussians);

    std::vector<int> output(num_gaussians, 0);

    float max_dist = 1.2f * glm::distance(bounds.first, bounds.second);

    for (auto const& g : data) {
        float d = glm::abs(glm::length(-cam_pos - g.pos));
        float d_normalized = n_buckets * glm::sqrt(d / max_dist);  // between 0 and n_buckets
        size_t d_int = glm::min(d_normalized, (float)n_buckets - 1);
        ++count[d_int];
        distances.push_back(d_int);
    }

    for (int i = 1; i < count.size(); ++i) {
        count[i] = count[i] + count[i - 1];
    }

    for (int i = num_gaussians - 1; i >= 0; --i) {
        size_t j = distances[i];
        --count[j];
        output[count[j]] = i;
    }

    glBindBuffer(GL_SHADER_STORAGE_BUFFER, index_ssbo);
    glBufferData(GL_SHADER_STORAGE_BUFFER,
                 output.size() * sizeof(int),
                 output.data(),
                 GL_DYNAMIC_COPY);
}

/*
 * Naive and slow sorting of Gaussians using `std::sort`.
 */
void App::sort() {
    glm::vec4 cam_pos = glm::vec4(cam.get_pos(), 1);
    auto comp = [&](int i1, int i2) {
        return 0 > (glm::length(-cam_pos - data[i1].pos) - glm::length(-cam_pos - data[i2].pos));
    };
    std::sort(indices.begin(), indices.end(), comp);

    glBindBuffer(GL_SHADER_STORAGE_BUFFER, index_ssbo);
    glBufferData(GL_SHADER_STORAGE_BUFFER,
                 indices.size() * sizeof(int),
                 indices.data(),
                 GL_DYNAMIC_COPY);
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
        time = glfwGetTime();
        glfwSwapBuffers(win);
        glfwPollEvents();
        process_inputs();
        draw();
        ++frame;
        time_delta = glfwGetTime() - time;
    }
}

void App::process_inputs() {
    if (glfwGetKey(win, GLFW_KEY_X) == GLFW_PRESS) {
        sort();
    }
    if (glfwGetKey(win, GLFW_KEY_C) == GLFW_PRESS) {
        csort();
    }

    float speed = 1.5f;
    if (glfwGetKey(win, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS) {
        speed *= 5.0f;
    }
    speed *= time_delta;

    if (glfwGetKey(win, GLFW_KEY_Q) == GLFW_PRESS) {
        glfwSetWindowShouldClose(win, GLFW_TRUE);
    }

    if (glfwGetKey(win, GLFW_KEY_W) == GLFW_PRESS) {
        cam.translate(speed * cam.forward());
    }
    if (glfwGetKey(win, GLFW_KEY_S) == GLFW_PRESS) {
        cam.translate(-speed * cam.forward());
    }
    if (glfwGetKey(win, GLFW_KEY_A) == GLFW_PRESS) {
        cam.translate(-speed * cam.right());
    }
    if (glfwGetKey(win, GLFW_KEY_D) == GLFW_PRESS) {
        cam.translate(speed * cam.right());
    }
    if (glfwGetKey(win, GLFW_KEY_SPACE) == GLFW_PRESS) {
        cam.translate(speed * cam.up());
    }
    if (glfwGetKey(win, GLFW_KEY_LEFT_CONTROL) == GLFW_PRESS) {
        cam.translate(-speed * cam.up());
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
    glBlendFuncSeparate(GL_DST_ALPHA, GL_ONE, GL_ZERO, GL_ONE_MINUS_SRC_ALPHA);
    glBlendEquation(GL_ADD);

    glUseProgram(shader);

    int w, h;
    glfwGetFramebufferSize(win, &w, &h);
    cam.update_res(w, h);

    auto proj = cam.get_proj();
    auto view = cam.get_view();
    float viewport_size[] = {(float)w, (float)h};

    // Upload uniforms
    GLint loc_proj = glGetUniformLocation(shader, "proj");
    GLint loc_view = glGetUniformLocation(shader, "view");
    GLint loc_viewport_size = glGetUniformLocation(shader, "viewport_size");
    glUniformMatrix4fv(loc_proj, 1, GL_FALSE, &proj[0][0]);
    glUniformMatrix4fv(loc_view, 1, GL_FALSE, &view[0][0]);
    glUniform2fv(loc_viewport_size, 1, viewport_size);

    // Bind vertex buffer, Gaussian SSBO, and index SSBO
    glBindVertexArray(vao);
    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, gauss_ssbo);
    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 1, index_ssbo);

    if (shader == gaussian_shader) {
        // Instanced draw call for N screen-space quads.
        glDrawArraysInstanced(GL_TRIANGLE_FAN, 0, 4, num_gaussians);
    } else {
        // Instanced draw call for N points.
        glDrawArraysInstanced(GL_POINTS, 0, 1, num_gaussians);
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
