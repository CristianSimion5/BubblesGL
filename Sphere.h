#pragma once
#include <random>
#include <GL/glew.h>
#include <glm/mat4x4.hpp>

class Sphere {
public:
    static GLuint VaoId, VboId, VboNormId, EboId;
    static GLfloat BaseRadius;
    static GLint parallelCount, meridianCount;
    static std::vector<GLuint> indices;
    static std::random_device rs;
    static std::default_random_engine generator;
    static std::uniform_real_distribution<float> distribution;

    static void InitVertex();
    static void DestroyVertex();

    bool CanCollision = false;

private:
    glm::mat4 MatrPos;
    glm::mat4 MatrScale;
    glm::vec3 Velocity;
    glm::vec3 Force;
    glm::vec3 Color;

    GLfloat Radius;

public:
    Sphere(GLfloat x, GLfloat y, GLfloat z, GLfloat vx, GLfloat vy, GLfloat vz,
        GLfloat r, glm::vec3 Color);

    GLfloat GetRadius() const;
    glm::vec3 GetCoordinates() const;
    glm::vec3 GetVelocity() const;

    void RenderSphere(GLuint ShaderId) const;
    void Update(bool ChangeForce);
    void ChangeDirection(glm::vec3 diff, glm::vec3 normal);
};