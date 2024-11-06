#include "camera.hpp"

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/quaternion.hpp>
#include <iostream>

namespace splat {

glm::mat4 Camera::get_view() const {
    glm::quat rot{euler_angles};
    glm::mat4 view_mat = glm::mat4_cast(glm::conjugate(rot));
    return glm::translate(view_mat, pos);
}

glm::mat4 Camera::get_proj() const {
    return glm::perspective(glm::radians(90.0f), 800.0f / 600.0f, 0.1f, 1000.0f);
}

void Camera::translate(glm::vec3 delta) {
    pos += speed * delta;
}

}  // namespace splat
