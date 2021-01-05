#pragma once
#include <vector>
#include <random>

#include "Circle.h"

class Scene {
private:
    std::vector <Circle> VC;
    std::random_device rs;
    std::default_random_engine generator;
    std::uniform_real_distribution<float> distribution;
    std::uniform_real_distribution<float> distribution2;
    std::uniform_real_distribution<float> distribution3;
    
    int WaitingCircles;
    int FrameCount;

private:
    GLfloat GetDistance(const Circle &c1, const Circle &c2) const;
    bool CheckCollision(const Circle &c1, const Circle &c2) const;
    void ResolveCollision(Circle &c1, Circle &c2);

public:
    Scene(int NumCircles, bool SameRadius = false);

    void AddCircle(GLfloat x, GLfloat y, GLfloat z, GLfloat vx, GLfloat vy, GLfloat vz, 
        GLfloat r, glm::vec3 c);
    void AddCircle(Circle c);
    void AddRandomCircle();

    void RenderScene(GLint ShaderId) const;
    void CheckCollisions();
    void Update();
};