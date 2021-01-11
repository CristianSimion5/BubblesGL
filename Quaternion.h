#pragma once

#include <glm/glm.hpp>
#include <ostream>

class Quaternion {
private:
    float x, y, z, w;

public:
    Quaternion();
    Quaternion(glm::vec3 Point);
    Quaternion(glm::vec3 Axis, float Angle);
    Quaternion(float w, float x, float y, float z);

    float SquareNorm();
    Quaternion Conjugate();
    Quaternion FromAxisAndAngle(glm::vec3 Axis, float Angle);
    void Rotate(glm::vec3 Axis, float Angle);
    glm::mat4 ToRotationMatrix();

    Quaternion operator*(const Quaternion& Q) const;
    operator glm::vec3() const;

    friend std::ostream& operator<<(std::ostream& out, const Quaternion& Q);
};