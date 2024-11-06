#ifndef CAMERA_HPP
#define CAMERA_HPP

#include <glm/matrix.hpp>
#include <glm/vec3.hpp>

namespace splat {

class Camera {
   public:
    glm::mat4 get_view() const;
    glm::mat4 get_proj() const;
    void translate(glm::vec3 delta);

   private:
    glm::vec3 pos = {0, 0, 0};
    glm::vec3 euler_angles = {0, 0, 0};
    float speed = 0.2f;
};

}  // namespace splat

#endif // CAMERA_HPP
