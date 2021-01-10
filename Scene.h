#pragma once
#include <vector>
#include <random>
#include <chrono>

#include "Circle.h"
#include "TrackCamera.h"

#include <glm/gtc/matrix_transform.hpp>

typedef std::chrono::steady_clock timer;

class Scene {
private:
    std::vector <Circle> VC;
    std::random_device rs;
    std::default_random_engine generator;
    std::uniform_real_distribution<float> distribution;
    std::uniform_real_distribution<float> distribution2;
    std::uniform_real_distribution<float> distribution3;
    
    const glm::vec3 OutlineColor = glm::vec3(0.1f, 0.1f, 0.1f);
    glm::vec3 LightPosition, LightColor;
    
    
    TrackCamera Camera;
    glm::vec3 P1, P2;
    bool MouseDrag = false;
    float HalfWidth, HalfHeight, TrackRadiusSq;
    timer::time_point StartIdle = timer::now();

    GLuint VaoId, VboId, EboId;
    GLfloat XBounds[2] = { -1.f, 1.f }, YBounds[2] = { -1.f, 1.f }, ZBounds[2] = { -1.f, 1.f };

    int WaitingCircles;
    int FrameCount;

private:
    GLfloat GetDistance(const Circle &c1, const Circle &c2) const;
    bool CheckCollision(const Circle &c1, const Circle &c2) const;
    void ResolveCollision(Circle &c1, Circle &c2);
    void SortByDepth();

public:
    Scene(int NumCircles, bool SameRadius = false);

    void CreateVertexBuffers();
    void AddCircle(GLfloat x, GLfloat y, GLfloat z, GLfloat vx, GLfloat vy, GLfloat vz, 
        GLfloat r, glm::vec3 c);
    void AddCircle(Circle c);
    void AddRandomCircle();

    void RenderScene(GLint ShaderId);
    void CheckCollisions();
    void Update();

    void DisableMouseDrag();
    void EnableMouseDrag();
    void SetMouseP1(int x, int y);
    void SetMouseP2(int x, int y);
    void PrintPoints();
    void SetScreenSize(int width, int height);
    void ComputeZ(glm::vec3& P);
    void DragRotate();
    void ResetTimer();
};