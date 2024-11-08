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
    float aspect = (float)width / (float)height;
    return glm::perspective(glm::radians(90.0f), aspect, 0.1f, 1000.0f);
}

void Camera::translate(glm::vec3 delta) {
    pos += speed * delta;
}

void Camera::update_rot(double mouse_x_new, double mouse_y_new) {
    double mouse_x_old = mouse_x;
    double mouse_y_old = mouse_y;

    if (mouse_x_old >= 0) {
        double dx = mouse_x_new - mouse_x_old;
        double dy = mouse_y_new - mouse_y_old;

        double f = -0.005f;

        euler_angles.y += f * dx;
        euler_angles.x += f * dy;
        euler_angles.x = glm::clamp(euler_angles.x, glm::radians(-90.0f), glm::radians(90.0f));
    }

    mouse_x = mouse_x_new;
    mouse_y = mouse_y_new;
}

void Camera::reset_mouse() {
    mouse_x = -100;
    mouse_y = -100;
}

void Camera::update_res(size_t w, size_t h) {
    width = w;
    height = h;
}

}  // namespace splat