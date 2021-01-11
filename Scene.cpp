#include "Scene.h"

#include <iostream>
#include <vector>
#include <algorithm>

#include "stb_image.h"
#include <glm/gtc/matrix_transform.hpp>

Scene::Scene(int NumSpheres, bool SameRadius) {
    generator = std::default_random_engine(rs());
    distribution = std::uniform_real_distribution<float>(0.001f, 0.01f);
    if (SameRadius)
        distribution2 = std::uniform_real_distribution<float>(1.f, 1.f);
    else
        distribution2 = std::uniform_real_distribution<float>(0.5f, 1.f);
    distribution3 = std::uniform_real_distribution<float>(0.f, 1.f);

    AddRandomSphere();
    WaitingSpheres = NumSpheres - 1;
    FrameCount = 0;
}

Scene::~Scene() {
    // Distruge VBO-urile si VAO-urile
    glDeleteBuffers(2, VboId);
    glDeleteBuffers(1, &EboId);
    glDeleteTextures(1, &TexId);
    glDeleteTextures(1, &NormalMapId);
    glDeleteVertexArrays(2, VaoId);
}

void Scene::CreateVertexBuffers() {
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

    GLfloat FloorVertices[] = {
        XBounds[0], YBounds[0], ZBounds[0], 0.f, 0.f,
        XBounds[0], YBounds[0], ZBounds[1], 0.f, 1.f,
        XBounds[1], YBounds[0], ZBounds[1], 1.f, 1.f,
        XBounds[1], YBounds[0], ZBounds[0], 1.f, 0.f,
    };
    GLubyte FloorIndices[] = {
        0, 1, 2,
        2, 3, 0
    };

    glGenVertexArrays(2, VaoId);
    glBindVertexArray(VaoId[0]);
    glGenBuffers(2, VboId);

    // Varfurile Conturului
    glBindBuffer(GL_ARRAY_BUFFER, VboId[0]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(BoxOutline), &BoxOutline[0], GL_STATIC_DRAW);

    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), 0);

    // Varfurile Podelei
    glBindVertexArray(VaoId[1]);
    glBindBuffer(GL_ARRAY_BUFFER, VboId[1]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(FloorVertices), &FloorVertices[0], GL_STATIC_DRAW);

    // Indicii Podelei
    glGenBuffers(1, &EboId);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EboId);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER,
        sizeof(FloorIndices), &FloorIndices[0], GL_STATIC_DRAW);

    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat), 0);

    glEnableVertexAttribArray(2);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat),
        (void*)(3 * sizeof(float)));

    TexId = LoadTexture("textures/marble_texture.jpg");
    NormalMapId = LoadTexture("textures/marble_normal.jpg");

    // Calcul Tangente si Bitangente
    // https://learnopengl.com/Advanced-Lighting/Normal-Mapping
    // http://www.opengl-tutorial.org/intermediate-tutorials/tutorial-13-normal-mapping/
    std::vector<glm::vec3> vertices = {
        glm::vec3(XBounds[0], YBounds[0], ZBounds[0]),
        glm::vec3(XBounds[0], YBounds[0], ZBounds[1]),
        glm::vec3(XBounds[1], YBounds[0], ZBounds[1]),
        glm::vec3(XBounds[1], YBounds[0], ZBounds[0])
    };
    std::vector<glm::vec2> uvs = {
        glm::vec2(0.f, 0.f),
        glm::vec2(0.f, 1.f),
        glm::vec2(1.f, 1.f),
        glm::vec2(1.f, 0.f)
    };
    glm::vec3 normal(0.f, 1.f, 0.f);

    std::vector<glm::vec3> tangents(vertices.size()), bitangents(vertices.size());

    int length = sizeof(FloorIndices) / sizeof(FloorIndices[0]);
    for (int i = 0; i < length; i += 3) {
        auto i1 = FloorIndices[i];
        auto i2 = FloorIndices[i + 1];
        auto i3 = FloorIndices[i + 2];

        glm::vec3& v0 = vertices[i1];
        glm::vec3& v1 = vertices[i2];
        glm::vec3& v2 = vertices[i3];

        glm::vec2& uv0 = uvs[i1];
        glm::vec2& uv1 = uvs[i2];
        glm::vec2& uv2 = uvs[i3];

        glm::vec3 deltaPos1 = v1 - v0;
        glm::vec3 deltaPos2 = v2 - v0;

        glm::vec2 deltaUV1 = uv1 - uv0;
        glm::vec2 deltaUV2 = uv2 - uv0;

        float invDet = 1.f / (deltaUV1.x * deltaUV2.y - deltaUV2.x * deltaUV1.y);
        glm::vec3 tangent = invDet * (deltaPos1 * deltaUV2.y - deltaPos2 * deltaUV1.y);
        glm::vec3 bitangent = invDet * (deltaPos2 * deltaUV1.x - deltaPos1 * deltaUV2.x);

        tangents[i1] = tangent;
        tangents[i2] = tangent;
        tangents[i3] = tangent;

        bitangents[i1] = bitangent;
        bitangents[i2] = bitangent;
        bitangents[i3] = bitangent;
    }

    GLuint tgBuffer, btgBuffer;
    glGenBuffers(1, &tgBuffer);
    glBindBuffer(GL_ARRAY_BUFFER, tgBuffer);
    glBufferData(GL_ARRAY_BUFFER,
        tangents.size() * sizeof(glm::vec3), &tangents[0], GL_STATIC_DRAW);

    glEnableVertexAttribArray(3);
    glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, sizeof(glm::vec3), 0);

    glGenBuffers(1, &btgBuffer);
    glBindBuffer(GL_ARRAY_BUFFER, btgBuffer);
    glBufferData(GL_ARRAY_BUFFER,
        bitangents.size() * sizeof(glm::vec3), &bitangents[0], GL_STATIC_DRAW);

    glEnableVertexAttribArray(4);
    glVertexAttribPointer(4, 3, GL_FLOAT, GL_FALSE, sizeof(glm::vec3), 0);
}

void Scene::AddSphere(GLfloat x, GLfloat y, GLfloat z, GLfloat vx, GLfloat vy, GLfloat vz,
    GLfloat r, glm::vec3 c) {
    VC.push_back(Sphere(x, y, z, vx, vy, vz, r, c));
}

void Scene::AddSphere(Sphere c) {
    VC.push_back(c);
}

void Scene::AddRandomSphere() {
    GLfloat x, y, z, vx, vy, vz, r;
    vx = distribution(generator);
    vy = distribution(generator);
    vz = distribution(generator);
    r = distribution2(generator);

    x = -1.f + r * Sphere::BaseRadius;
    y = -1.f + r * Sphere::BaseRadius;
    z = -1.f + r * Sphere::BaseRadius;

    glm::vec3 c(distribution3(generator), distribution3(generator), distribution3(generator));
    VC.push_back(Sphere(x, y, z, vx, vy, vz, r, c));
}

void Scene::RenderScene(GLint ShaderId) {
    glUseProgram(ShaderId);

    // Seteaza unitatile de texturare
    glUniform1i(glGetUniformLocation(ShaderId, "uTexture"), 0);
    glUniform1i(glGetUniformLocation(ShaderId, "uNormalMap"), 1);

    glBindVertexArray(VaoId[0]);

    GLuint lColLocation = glGetUniformLocation(ShaderId, "lightColor");
    GLuint lPosLocation = glGetUniformLocation(ShaderId, "lightPos");
    GLuint cPosLocation = glGetUniformLocation(ShaderId, "cameraPos");
    GLint mModelLoc = glGetUniformLocation(ShaderId, "mModel");
    GLint mViewLoc = glGetUniformLocation(ShaderId, "mView");
    GLint mProjectionLoc = glGetUniformLocation(ShaderId, "mProjection");
    GLint uColorLoc = glGetUniformLocation(ShaderId, "uColor");
    GLint uIsFloorLoc = glGetUniformLocation(ShaderId, "uIsFloor");

    glm::mat4 mModel = glm::mat4(1.f);
    glm::mat4 mView = Camera.ToViewMatrix();
    glm::mat4 mProjection = glm::perspective(
        glm::radians(45.f), HalfWidth / HalfHeight, 1.f, 20.f);

    glUniform3fv(lColLocation, 1, &LightColor[0]);
    glUniform3fv(lPosLocation, 1, &LightPosition[0]);
    glUniform3fv(cPosLocation, 1, &Camera.GetPosition()[0]);
    glUniformMatrix4fv(mModelLoc, 1, GL_FALSE, &mModel[0][0]);
    glUniformMatrix4fv(mViewLoc, 1, GL_FALSE, &mView[0][0]);
    glUniformMatrix4fv(mProjectionLoc, 1, GL_FALSE, &mProjection[0][0]);
    glUniform3fv(uColorLoc, 1, &OutlineColor[0]);
    glUniform1i(uIsFloorLoc, true);

    glLineWidth(2.f);
    glDrawArrays(GL_LINES, 0, 24);

    // Seteaza texturile corespunzatoare culorii/normalelor
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, TexId);
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, NormalMapId);
    glBindVertexArray(VaoId[1]);

    glDrawElements(GL_TRIANGLES, 2 * 3, GL_UNSIGNED_BYTE, 0);

    glBindTexture(GL_TEXTURE_2D, 0);
    glUniform1i(uIsFloorLoc, false);

    // Opreste masca de adancime pentru a desena corect obiectele transparente
    glDepthMask(GL_FALSE);
    glBindVertexArray(Sphere::VaoId);
    for (const Sphere& Sphere : VC) {
        Sphere.RenderSphere(ShaderId);
    }
    glDepthMask(GL_TRUE);

    glUseProgram(0);
}

GLfloat Scene::GetDistance(const Sphere& c1, const Sphere& c2) const {
    glm::vec3 diff = c1.GetCoordinates() - c2.GetCoordinates();
    return glm::sqrt(glm::dot(diff, diff));
}

bool Scene::CheckCollision(const Sphere& c1, const Sphere& c2) const {
    return GetDistance(c1, c2) < (c1.GetRadius() + c2.GetRadius());
}

void Scene::ResolveCollision(Sphere& c1, Sphere& c2) {
    glm::vec3 normal = c1.GetCoordinates() - c2.GetCoordinates();
    glm::vec3 v_normal = c1.GetVelocity() - c2.GetVelocity();

    float resistance = 4e-1f;
    glm::vec3 diff = (1 - resistance) * glm::dot(v_normal, normal) / glm::dot(normal, normal) * normal;

    c1.ChangeDirection(diff, normal);
    c2.ChangeDirection(-diff, -normal);
}

void Scene::SortByDepth() {
    // Sorteaza sferele in ordine back-to-front, pentru desenare corecta atunci cand avem transparenta
    std::sort(VC.begin(), VC.end(), [this](const Sphere& c1, const Sphere& c2) {
        return (glm::distance(c1.GetCoordinates(), Camera.GetPosition()) + c1.GetRadius() >
            glm::distance(c2.GetCoordinates(), Camera.GetPosition()) + c2.GetRadius());
        });
}

GLuint Scene::LoadTexture(const char* path) {
    // Texturare pentru Normal Mapping
    // Realizat urmand tutorialul https://learnopengl.com/Getting-started/Textures
    GLuint texture;
    glGenTextures(1, &texture);

    int width, height, nrChannels;
    GLubyte* data = stbi_load(path, &width, &height, &nrChannels, 0);
    if (data) {
        glBindTexture(GL_TEXTURE_2D, texture);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        //std::cout << width << ' ' << height << ' ' << nrChannels;
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
    }
    else {
        std::cout << "Failed to load texture\n";
    }
    stbi_image_free(data);
    glBindTexture(GL_TEXTURE_2D, 0);

    return texture;
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
    for (Sphere& Sphere : VC) {
        Sphere.Update(ChangeForce);
    }
    CheckCollisions();
    FrameCount = (FrameCount + 1) % 150;
    if (WaitingSpheres > 0 && FrameCount % 30 == 0) {
        AddRandomSphere();
        WaitingSpheres--;
    }
    SortByDepth();
    auto d = timer::now() - StartIdle;
    if (d.count() * 1e-9f > 4.f)
        Camera.RotateCamera(
            glm::vec3(0.f, 1.f, 0.f),
            glm::pi<float>() / 3000);
    if (MouseDrag) {
        DragRotate();
    }
}

void Scene::DisableMouseDrag() {
    MouseDrag = false;
}

void Scene::EnableMouseDrag() {
    MouseDrag = true;
}

void Scene::SetMouseP1(int x, int y) {
    P1 = glm::vec3(HalfWidth - x, y - HalfHeight, 0.f);
    ResetTimer();
}

void Scene::SetMouseP2(int x, int y) {
    P2 = glm::vec3(HalfWidth - x, y - HalfHeight, 0.f);
    ResetTimer();
}

void Scene::Scroll(bool up) {
    if (up)
        Camera.UpdateRadius(-0.01f);
    else
        Camera.UpdateRadius(0.01f);
}

void Scene::SetScreenSize(int width, int height) {
    HalfWidth = width / 2.f;
    HalfHeight = height / 2.f;
    float TrackRadius = glm::min(HalfWidth, HalfHeight);
    TrackRadiusSq = TrackRadius * TrackRadius;
}

void Scene::ComputeZ(glm::vec3& P) {
    float SqX = P.x * P.x, SqY = P.y * P.y;
    if (SqX + SqY <= TrackRadiusSq) {
        P.z = glm::sqrt(TrackRadiusSq - SqX - SqY);
    }
    else
        P.z = 0.f;
}

void Scene::DragRotate() {
    // Functionalitate implementata urmarind urmatoarele tutoriale
    // https://pixeladventuresweb.wordpress.com/2016/10/04/arcball-controller/
    // http://asliceofrendering.com/camera/2019/11/30/ArcballCamera/
    // https://en.wikibooks.org/wiki/OpenGL_Programming/Modern_OpenGL_Tutorial_Arcball
    if (glm::distance(glm::vec2(P1), glm::vec2(P2)) < 0.1f)
        return;

    ComputeZ(P1);
    ComputeZ(P2);

    glm::vec3 P1n = glm::normalize(P1), P2n = glm::normalize(P2);
    glm::vec3 axis = glm::normalize(glm::cross(P1n, P2n));
    float angle = glm::acos(glm::dot(P1n, P2n));
    if (glm::isnan(angle)) {
        angle = 0;
    }

    Camera.RotateCamera(glm::inverse(glm::mat3(Camera.ToViewMatrix())) * axis, angle);
    P1 = P2;
}

void Scene::ResetTimer() {
    StartIdle = timer::now();
}