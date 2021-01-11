#include "Quaternion.h"

Quaternion::Quaternion() : x(0.f), y(0.f), z(0.f), w(1.f) {}

Quaternion::Quaternion(glm::vec3 Point) :
    x(Point.x), y(Point.y), z(Point.z), w(0.f) {}

Quaternion::Quaternion(glm::vec3 Axis, float Angle) {
    *this = FromAxisAndAngle(Axis, Angle);
}

Quaternion::Quaternion(float w, float x, float y, float z) :
    x(x), y(y), z(z), w(w) {}

float Quaternion::SquareNorm()
{
    return x * x + y * y + z * z + w * w;
}

Quaternion Quaternion::Conjugate() {
    return Quaternion(w, -x, -y, -z);
}

Quaternion Quaternion::FromAxisAndAngle(glm::vec3 Axis, float Angle) {
    float HalfAngle = Angle / 2.f;
    float SinHalf = glm::sin(HalfAngle);

    float new_w = glm::cos(HalfAngle);
    float new_x = Axis.x * SinHalf;
    float new_y = Axis.y * SinHalf;
    float new_z = Axis.z * SinHalf;

    return Quaternion(new_w, new_x, new_y, new_z);
}

void Quaternion::Rotate(glm::vec3 Axis, float Angle) {
    *this = FromAxisAndAngle(Axis, Angle) * (*this);
}

glm::mat4 Quaternion::ToRotationMatrix() {
    glm::mat4 rot(1.f);

    // Nota: matricea este in forma de coloana, deci indexarea se face dupa coloana, apoi dupa rand
    rot[0][0] = 1 - 2 * y * y - 2 * z * z;
    rot[0][1] = 2 * x * y + 2 * w * z;
    rot[0][2] = 2 * x * z - 2 * w * y;

    rot[1][0] = 2 * x * y - 2 * w * z;
    rot[1][1] = 1 - 2 * x * x - 2 * z * z;
    rot[1][2] = 2 * y * z + 2 * w * x;

    rot[2][0] = 2 * x * z + 2 * w * y;
    rot[2][1] = 2 * y * z - 2 * w * x;
    rot[2][2] = 1 - 2 * x * x - 2 * y * y;

    return rot;
}

Quaternion Quaternion::operator*(const Quaternion& Q) const {
    float new_w = w * Q.w - x * Q.x - y * Q.y - z * Q.z;
    float new_x = w * Q.x + x * Q.w + y * Q.z - z * Q.y;
    float new_y = w * Q.y - x * Q.z + y * Q.w + z * Q.x;
    float new_z = w * Q.z + x * Q.y - y * Q.x + z * Q.w;

    return Quaternion(new_w, new_x, new_y, new_z);
}

Quaternion::operator glm::vec3() const {
    return glm::vec3(x, y, z);
}

std::ostream& operator<<(std::ostream& out, const Quaternion& Q) {
    out << Q.w << ' ' << Q.x << ' ' << Q.y << ' ' << Q.z;
    return out;
}