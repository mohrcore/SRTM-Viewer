#pragma once

#include <glm/vec3.hpp>
#include <glm/vec2.hpp>
#include <glm/mat4x4.hpp>
#include <glm/gtc/quaternion.hpp>

namespace graphics {
    class Camera3D {
    private:
        glm::vec3 _position;
        glm::quat _rotation;
        float _fov;
        float _aspet_ratio;
        float _near;
        float _far;
        glm::mat4x4 _projection_matrix;
        glm::vec3 _forward;
        glm::vec3 _global_up;
    public:
        Camera3D();
        void setPosition(glm::vec3 position);
        void move(glm::vec3 delta);
        glm::vec3 getPosition() const;
        void setRotation(glm::quat roatation);
        void look(glm::vec3 head_movement);
        void lookAt(glm::vec3 point);
        glm::quat getRotation() const;
        void setProjection(float fov, float aspect_ratio, float near, float far);
        float getNearDistance() const;
        float getFarDistance() const;
        float getAspectRatio() const;
        float getFOV() const;
        glm::mat4x4 getProjectionMatrix() const;
        glm::vec3 getForwardVec() const;
        void setUpVector(glm::vec3 up);
        glm::vec3 getUpVector() const;
    };
}