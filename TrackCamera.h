#pragma once

#include <glm/glm.hpp>

class TrackCamera {
private:
    const glm::vec3 up = glm::vec3(0.f, 1.f, 0.f), forward = glm::vec3(0.f, 0.f, -1.f);

    glm::vec3 CameraPosition = glm::vec3(3.f, 2.f, 1.5f);
    float Radius;

public:
    glm::vec3 GetPosition() const;

    void RotateCamera(glm::vec3 Axis, float Angle);
    glm::mat4 ToViewMatrix(glm::vec3 Object = glm::vec3(0.f, 0.f, 0.f)) const;
};

