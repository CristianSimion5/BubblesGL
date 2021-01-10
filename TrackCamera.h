#pragma once

#include <glm/glm.hpp>

class TrackCamera {
private:
    glm::vec3 up = glm::vec3(0.f, 1.f, 0.f);

    glm::vec3 CameraPosition = glm::vec3(3.f, 2.f, 2.5f);
    float Radius = 1.f, MinRadius = .5f, MaxRadius = 2.f;

public:
    glm::vec3 GetPosition() const;

    void UpdateRadius(float Delta);
    void RotateCamera(glm::vec3 Axis, float Angle);
    glm::mat4 ToViewMatrix(glm::vec3 Object = glm::vec3(0.f, 0.f, 0.f)) const;
};

