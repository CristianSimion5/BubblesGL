#include "TrackCamera.h"

#include "Quaternion.h"

#include <glm/gtc/matrix_transform.hpp>

glm::vec3 TrackCamera::GetPosition() const {
    return CameraPosition;
}

void TrackCamera::RotateCamera(glm::vec3 Axis, float Angle) {
    Quaternion q(Axis, Angle);
    CameraPosition = q * CameraPosition * q.Conjugate();
    up = q * up * q.Conjugate();
}

glm::mat4 TrackCamera::ToViewMatrix(glm::vec3 Object) const {
    glm::vec3 Orientation = glm::normalize(CameraPosition - Object);
    glm::vec3 CameraUp = glm::cross(Orientation, glm::normalize(glm::cross(up, Orientation)));
    
    //float sgn = glm::sign(gl);
    return glm::lookAt(CameraPosition, Object, CameraUp);
}
