#include "Scene.h"

#include <iostream>
#include <vector>

Scene::Scene(int NumCircles, bool SameRadius) {
    generator = std::default_random_engine(rs());
    distribution = std::uniform_real_distribution<float>(0.001f, 0.01f);
    if (SameRadius)
        distribution2 = std::uniform_real_distribution<float>(1.f, 1.f);
    else
        distribution2 = std::uniform_real_distribution<float>(0.5f, 1.f);
    distribution3 = std::uniform_real_distribution<float>(0.f, 1.f);

    AddRandomCircle();
    WaitingCircles = NumCircles - 1;
    FrameCount = 0;
}

void Scene::AddCircle(GLfloat x, GLfloat y, GLfloat vx, GLfloat vy, GLfloat r, glm::vec3 c) {
    VC.push_back(Circle(x, y, vx, vy, r, c));
}

void Scene::AddCircle(Circle c) {
    VC.push_back(c);
}

void Scene::AddRandomCircle() {
    GLfloat x, y, vx, vy, r;
    vx = distribution(generator);
    vy = distribution(generator);
    r = distribution2(generator);
    x = -1.f + r * Circle::BaseRadius;
    y = -1.f + r * Circle::BaseRadius;
    glm::vec3 c(distribution3(generator), distribution3(generator), distribution3(generator));
    VC.push_back(Circle(x, y, vx, vy, r, c));
}

void Scene::RenderScene(GLint ShaderId) const {
    for (const Circle& circle : VC) {
        circle.RenderCircle(ShaderId);
    }
}

GLfloat Scene::GetDistance(const Circle &c1, const Circle &c2) const {
    glm::vec2 diff = c1.GetCoordinates() - c2.GetCoordinates();
    return glm::sqrt(glm::dot(diff, diff));
}

bool Scene::CheckCollision(const Circle &c1, const Circle &c2) const {
    return GetDistance(c1, c2) < (c1.GetRadius() + c2.GetRadius());
}

void Scene::ResolveCollision(Circle &c1, Circle &c2) {
    glm::vec2 normal = c1.GetCoordinates() - c2.GetCoordinates();
    glm::vec2 v_normal = c1.GetVelocity() - c2.GetVelocity();
    
    float resistance = 4e-1f;
    glm::vec2 diff = (1 - resistance) * glm::dot(v_normal, normal) / glm::dot(normal, normal) * normal;
    
    c1.ChangeDirection(diff, normal);
    c2.ChangeDirection(-diff, -normal);
}

void Scene::CheckCollisions() {
    std::vector <bool> HasCollisioned(VC.size(), false);
    for (int i = 0; i < VC.size(); i++) {
        for (int j = i + 1; j < VC.size(); j++) {
            if (CheckCollision(VC[i], VC[j])) {
                if (VC[i].CanCollision && VC[j].CanCollision)
                    ResolveCollision(VC[i], VC[j]);
                HasCollisioned[i] = true;
                HasCollisioned[j] = true;
            }
        }
        if (HasCollisioned[i] == false && VC[i].CanCollision == false) {
            VC[i].CanCollision = true;
        }
    }
}

void Scene::Update() {
    bool ChangeForce = (FrameCount % 150) == 0;
    for (Circle& circle : VC) {
        circle.Update(ChangeForce);
    }
    CheckCollisions();
    FrameCount = (FrameCount + 1) % 150;
    if (WaitingCircles > 0 && FrameCount % 30 == 0) {
        AddRandomCircle();
        WaitingCircles--;
    }
}