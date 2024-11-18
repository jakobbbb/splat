#ifndef APP_HPP
#define APP_HPP

#include "camera.hpp"

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <vector>
#include <utility>

int main(int argc, char** argv);

namespace splat {

struct Gaussian {
    // X, Y, Z, W=1
    glm::vec4 pos;
    // R, G, B, A
    glm::vec4 color;
    // 3D Covariance, as mat4 for alignment
    glm::mat4 sigma;
};

class App {
   public:
    App(char* ply_path);
    void run();

    const uint32_t WIDTH = 1280;
    const uint32_t HEIGHT = 720;

   private:
    GLFWwindow* win;
    double time;
    double time_delta;
    void init_window();
    void draw();
    void process_inputs();
    void load_data(char* ply_path);
    void load_shaders();

    std::vector<Gaussian> data = {};
    std::pair<glm::vec3, glm::vec3> bounds;

    std::vector<int> indices = {};
    void sort();
    void csort();

    uint32_t frame = 0;
    GLuint vertex_buffer;
    GLuint vao;
    GLuint gauss_ssbo;
    GLuint index_ssbo;
    GLuint point_shader;
    GLuint gaussian_shader;
    GLuint shader;
    size_t num_gaussians;

    Camera cam;
};
}  // namespace splat

#endif  // APP_HPP
