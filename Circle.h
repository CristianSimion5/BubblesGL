#pragma once
#include <random>
#include <GL/glew.h>
#include <glm/mat4x4.hpp>

class Circle {
public:
    static GLuint VboId, VaoId;
    static GLfloat BaseRadius;
    static std::random_device rs;
    static std::default_random_engine generator;
    static std::uniform_real_distribution<float> distribution;

    static void InitVertex();
    static void DestroyVertex();

    bool CanCollision = false;

private:
    glm::mat4 MatrPos;
    glm::mat4 MatrScale;
    glm::vec2 Velocity;
    glm::vec2 Force;
    glm::vec3 Color;

    GLfloat Radius;

public:
    Circle(GLfloat x, GLfloat y, GLfloat vx, GLfloat vy, GLfloat r, glm::vec3 Color);

    GLfloat GetRadius() const;
    glm::vec2 GetCoordinates() const;
    glm::vec2 GetVelocity() const;

    void RenderCircle(GLuint ShaderId) const;
    void Update(bool ChangeForce);
    void ChangeDirection(glm::vec2 diff, glm::vec2 normal);
};