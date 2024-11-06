#ifndef APP_HPP
#define APP_HPP

#include <GL/glew.h>
#include <GLFW/glfw3.h>

int main(int argc, char** argv);

namespace splat {
class App {
   public:
    App();
    void run();

    const uint32_t WIDTH = 1280;
    const uint32_t HEIGHT = 720;

   private:
    GLFWwindow* win;
    void init_window();
    void draw();
    void process_inputs();
    void load_data();
    void load_shaders();

    uint32_t frame = 0;
    GLuint vertex_buffer;
    GLuint point_shader;
};
}  // namespace splat

#endif  // APP_HPP
