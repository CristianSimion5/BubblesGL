#pragma once
#include <vector>
#include <random>
#include <chrono>

#include "Sphere.h"
#include "TrackCamera.h"

#include <glm/gtc/matrix_transform.hpp>

typedef std::chrono::steady_clock timer;

class Scene {
private:
    std::vector <Sphere> VC;
    std::random_device rs;
    std::default_random_engine generator;
    std::uniform_real_distribution<float> distribution;
    std::uniform_real_distribution<float> distribution2;
    std::uniform_real_distribution<float> distribution3;

    const glm::vec3 OutlineColor = glm::vec3(0.2f, 0.2f, 0.2f),
        LightColor = glm::vec3(1.f, 1.f, 1.f),
        LightPosition = glm::vec3(0.f, 1.f, 2.f);

    TrackCamera Camera;
    glm::vec3 P1, P2;
    bool MouseDrag = false;
    float HalfWidth, HalfHeight, TrackRadiusSq;
    timer::time_point StartIdle = timer::now();

    GLuint VaoId[2], VboId[2], EboId, TexId, NormalMapId;
    GLfloat XBounds[2] = { -1.f, 1.f },
        YBounds[2] = { -1.f, 1.f },
        ZBounds[2] = { -1.f, 1.f };

    int WaitingSpheres;
    int FrameCount;

private:
    GLfloat GetDistance(const Sphere& c1, const Sphere& c2) const;
    bool CheckCollision(const Sphere& c1, const Sphere& c2) const;
    void ResolveCollision(Sphere& c1, Sphere& c2);
    void SortByDepth();
    GLuint LoadTexture(const char* path);

public:
    Scene(int NumSpheres, bool SameRadius = false);
    ~Scene();

    void CreateVertexBuffers();
    void AddSphere(GLfloat x, GLfloat y, GLfloat z, GLfloat vx, GLfloat vy, GLfloat vz,
        GLfloat r, glm::vec3 c);
    void AddSphere(Sphere c);
    void AddRandomSphere();

    void RenderScene(GLint ShaderId);
    void CheckCollisions();
    void Update();

    void DisableMouseDrag();
    void EnableMouseDrag();
    void SetMouseP1(int x, int y);
    void SetMouseP2(int x, int y);
    void Scroll(bool up);
    void SetScreenSize(int width, int height);
    void ComputeZ(glm::vec3& P);
    void DragRotate();
    void ResetTimer();
};