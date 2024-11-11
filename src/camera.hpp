#ifndef CAMERA_HPP
#define CAMERA_HPP

#include <glm/matrix.hpp>
#include <glm/vec3.hpp>
#include <glm/gtc/quaternion.hpp>

namespace splat {

class Camera {
   public:
    glm::mat4 get_view() const;
    glm::mat4 get_proj() const;
    glm::mat4 get_rot() const;
    glm::vec3 get_pos() const;
    glm::vec3 forward() const;
    glm::vec3 right() const;
    glm::vec3 up() const;
    void translate(glm::vec3 delta);
    void update_rot(double mouse_x, double mouse_y);
    void reset_mouse();
    void update_res(size_t width, size_t height);

   private:
    glm::vec3 pos = {0, 0, 0};
    glm::vec3 euler_angles = {0, 0, glm::radians(180.0f)};
    float speed = 0.2f;
    double mouse_x;
    double mouse_y;
    size_t width = 1;
    size_t height = 1;
};

}  // namespace splat

#endif // CAMERA_HPP
