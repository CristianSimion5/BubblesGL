#include "Scene.h"

#include <iostream>
#include <vector>
#include <algorithm>

#include <glm/gtc/matrix_transform.hpp>

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

    LightPosition = glm::vec3(0.f, 1.f, 2.f);
    LightColor = glm::vec3(1.f, 1.f, 1.f);
}

void Scene::CreateVertexBuffers()
{
    GLfloat BoxOutline[] = {
        XBounds[0], YBounds[0], ZBounds[0],
        XBounds[0], YBounds[0], ZBounds[1],

        XBounds[0], YBounds[0], ZBounds[1],
        XBounds[1], YBounds[0], ZBounds[1],
        
        XBounds[1], YBounds[0], ZBounds[1],
        XBounds[1], YBounds[0], ZBounds[0],

        XBounds[1], YBounds[0], ZBounds[0],
        XBounds[0], YBounds[0], ZBounds[0],

        XBounds[0], YBounds[1], ZBounds[0],
        XBounds[0], YBounds[1], ZBounds[1],
        
        XBounds[0], YBounds[1], ZBounds[1],
        XBounds[1], YBounds[1], ZBounds[1],
        
        XBounds[1], YBounds[1], ZBounds[1],
        XBounds[1], YBounds[1], ZBounds[0],
        
        XBounds[1], YBounds[1], ZBounds[0],
        XBounds[0], YBounds[1], ZBounds[0],

        XBounds[0], YBounds[0], ZBounds[0],
        XBounds[0], YBounds[1], ZBounds[0],

        XBounds[0], YBounds[0], ZBounds[1],
        XBounds[0], YBounds[1], ZBounds[1],

        XBounds[1], YBounds[0], ZBounds[1],
        XBounds[1], YBounds[1], ZBounds[1],

        XBounds[1], YBounds[0], ZBounds[0],
        XBounds[1], YBounds[1], ZBounds[0]
    };

    GLubyte FloorIndices[] = {
        0, 1, 3,
        3, 5, 0
    };

    glGenVertexArrays(1, &VaoId);
    glBindVertexArray(VaoId);

    glGenBuffers(1, &VboId);
    glBindBuffer(GL_ARRAY_BUFFER, VboId);
    glBufferData(GL_ARRAY_BUFFER, sizeof(BoxOutline), &BoxOutline[0], GL_STATIC_DRAW);

    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), 0);

    glGenBuffers(1, &EboId);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EboId);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(FloorIndices), &FloorIndices[0], GL_STATIC_DRAW);
}

void Scene::AddCircle(GLfloat x, GLfloat y, GLfloat z, GLfloat vx, GLfloat vy, GLfloat vz,
        GLfloat r, glm::vec3 c) {
    VC.push_back(Circle(x, y, z, vx, vy, vz, r, c));
}

void Scene::AddCircle(Circle c) {
    VC.push_back(c);
}

void Scene::AddRandomCircle() {
    GLfloat x, y, z, vx, vy, vz, r;
    vx = distribution(generator);
    vy = distribution(generator);
    vz = distribution(generator);
    r = distribution2(generator);

    x = -1.f + r * Circle::BaseRadius;
    y = -1.f + r * Circle::BaseRadius;
    z = -1.f + r * Circle::BaseRadius;
    
    glm::vec3 c(distribution3(generator), distribution3(generator), distribution3(generator));
    VC.push_back(Circle(x, y, z, vx, vy, vz, r, c));
}

void Scene::RenderScene(GLint ShaderId) {
    glUseProgram(ShaderId);
    glBindVertexArray(VaoId);

    GLuint lColLocation = glGetUniformLocation(ShaderId, "lightColor");
    glUniform3fv(lColLocation, 1, &LightColor[0]);

    GLuint lPosLocation = glGetUniformLocation(ShaderId, "lightPos");
    glUniform3fv(lPosLocation, 1, &LightPosition[0]);

    GLuint cPosLocation = glGetUniformLocation(ShaderId, "cameraPos");
    glUniform3fv(cPosLocation, 1, &Camera.GetPosition()[0]);

    glm::mat4 mModel = glm::mat4(1.f);
    GLint mModelLoc = glGetUniformLocation(ShaderId, "mModel");
    glUniformMatrix4fv(mModelLoc, 1, GL_FALSE, &mModel[0][0]);

    glm::mat4 mView = Camera.ToViewMatrix();
    GLint mViewLoc = glGetUniformLocation(ShaderId, "mView");
    glUniformMatrix4fv(mViewLoc, 1, GL_FALSE, &mView[0][0]);

    glm::mat4 mProjection = glm::perspective(glm::radians(45.f), 1.f, 1.f, 10.f);
    GLint mProjectionLoc = glGetUniformLocation(ShaderId, "mProjection");
    glUniformMatrix4fv(mProjectionLoc, 1, GL_FALSE, &mProjection[0][0]);

    GLint uColorLoc = glGetUniformLocation(ShaderId, "uColor");
    glUniform3fv(uColorLoc, 1, &OutlineColor[0]);
    
    GLint uIsFloorLoc = glGetUniformLocation(ShaderId, "uIsFloor");

    glUniform1i(uIsFloorLoc, true);
    
    glLineWidth(2.f);
    glDrawArrays(GL_LINES, 0, 24);
    glDrawElements(GL_TRIANGLES, 2 * 3, GL_UNSIGNED_BYTE, 0);
    
    glUniform1i(uIsFloorLoc, false);

    glDepthMask(GL_FALSE);
    glBindVertexArray(Circle::VaoId);
    for (const Circle& circle : VC) {
        circle.RenderCircle(ShaderId);
    }
    glDepthMask(GL_TRUE);

    glUseProgram(0);
}

GLfloat Scene::GetDistance(const Circle &c1, const Circle &c2) const {
    glm::vec3 diff = c1.GetCoordinates() - c2.GetCoordinates();
    return glm::sqrt(glm::dot(diff, diff));
}

bool Scene::CheckCollision(const Circle &c1, const Circle &c2) const {
    return GetDistance(c1, c2) < (c1.GetRadius() + c2.GetRadius());
}

void Scene::ResolveCollision(Circle &c1, Circle &c2) {
    glm::vec3 normal = c1.GetCoordinates() - c2.GetCoordinates();
    glm::vec3 v_normal = c1.GetVelocity() - c2.GetVelocity();
    
    float resistance = 4e-1f;
    glm::vec3 diff = (1 - resistance) * glm::dot(v_normal, normal) / glm::dot(normal, normal) * normal;
    
    c1.ChangeDirection(diff, normal);
    c2.ChangeDirection(-diff, -normal);
}

void Scene::SortByDepth() {
    std::sort(VC.begin(), VC.end(), [this](const Circle& c1, const Circle& c2) {
        return (glm::distance(c1.GetCoordinates(), Camera.GetPosition()) + c1.GetRadius() >
                glm::distance(c2.GetCoordinates(), Camera.GetPosition()) + c2.GetRadius());
        });
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
    SortByDepth();
    Camera.RotateCamera(glm::normalize(glm::vec3(0.f, 1.f, 0.f)), glm::pi<float>() / 100);
}