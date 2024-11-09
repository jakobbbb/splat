#ifndef APP_HPP
#define APP_HPP

#include "camera.hpp"

#include <GL/glew.h>
#include <GLFW/glfw3.h>

int main(int argc, char** argv);

namespace splat {

struct Gaussian {
    glm::vec4 pos;
    glm::vec4 color;
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
    void init_window();
    void draw();
    void process_inputs();
    void load_data(char* ply_path);
    void load_shaders();

    uint32_t frame = 0;
    GLuint vertex_buffer;
    GLuint vao;
    GLuint ssbo_buf;
    GLuint point_shader;
    GLuint gaussian_shader;
    GLuint shader;
    size_t num_gaussians;

    Camera cam;
};
}  // namespace splat

#endif  // APP_HPP
