#include "camera3d.h"

#include <cmath>

using namespace graphics;

Camera3D::Camera3D() :
    _global_up {0.0f, 1.0f, 0.0},
    _forward {0.0f, 0.0f, -1.0f}
{

}

void Camera3D::setPosition(glm::vec3 position) {
    this->_position = position;
}

glm::vec3 Camera3D::getPosition() const {
    return this->_position;
}

void Camera3D::move(glm::vec3 delta) {
    this->_position += delta;
}

void Camera3D::setRotation(glm::quat rotation) {
    this->_rotation = rotation;
}

//Arcball-style head controls
/* void Camera3D::look(glm::vec3 head_movement) {
    glm::vec3 upvec = this->_rotation * glm::vec3 {0.0, 1.0, 0.0};
    glm::vec3 direction_ymoved = glm::normalize(this->_rotation * glm::vec3 {0.0, 0.0 - head_movement.y, -1.0});
    glm::vec3 direction = {
        glm::cos(head_movement.x) * direction_ymoved.x - glm::sin(head_movement.x) * direction_ymoved.z,
        direction_ymoved.y,
        glm::sin(head_movement.x) * direction_ymoved.x + glm::cos(head_movement.x) * direction_ymoved.z,
    };
    this->_rotation = glm::quatLookAt(direction, upvec);
} */

void Camera3D::look(glm::vec3 head_movement) {
    glm::quat forward_rotation = glm::rotate(glm::identity<glm::quat>(), head_movement.x, this->_global_up);
    glm::vec3 y_rotation_axis = glm::normalize(glm::cross(this->_forward, this->_global_up));
    forward_rotation = glm::rotate(forward_rotation, head_movement.y, y_rotation_axis);
    this->_forward = forward_rotation * this->_forward;
    this->_rotation = glm::quatLookAt(this->_forward, this->_global_up);
}

void Camera3D::lookAt(glm::vec3 point) {
    glm::vec3 fwd = glm::normalize(point - this->_position);
    this->_rotation = glm::quatLookAt(fwd, this->_global_up);
    this->_forward = fwd;
}

glm::quat Camera3D::getRotation() const {
    return this->_rotation;
}

void Camera3D::setProjection(float fov, float aspect_ratio, float near, float far) {
    this->_projection_matrix = glm::perspective(fov, aspect_ratio, near, far);
    this->_fov = fov;
    this->_aspet_ratio = aspect_ratio;
    this->_near = near;
    this->_far = far;
}

float Camera3D::getFOV() const {
    return this->_fov;
}

float Camera3D::getAspectRatio() const {
    return this->_aspet_ratio;
}

float Camera3D::getNearDistance() const {
    return this->_near;
}

float Camera3D::getFarDistance() const {
    return this->_far;
}

glm::mat4x4 Camera3D::getProjectionMatrix() const {
    return this->_projection_matrix;
}

glm::vec3 Camera3D::getForwardVec() const {
    return this->_forward;
}

void Camera3D::setUpVector(glm::vec3 up) {
    this->_global_up = up;
    this->look({0.0f, 0.0f, 0.0f}); //Update view
}

glm::vec3 Camera3D::getUpVector() const {
    return this->_global_up;
}