#include "Sphere.h"

#include <GL/freeglut.h>
#include <glm/gtc/matrix_transform.hpp>

// Initializarea datelor membre statice
GLuint Sphere::VaoId;
GLuint Sphere::VboId;
GLuint Sphere::VboNormId;
GLuint Sphere::EboId;
std::vector<GLuint> Sphere::indices;

GLfloat Sphere::BaseRadius = 0.3f;
GLint Sphere::parallelCount = 50;
GLint Sphere::meridianCount = 50;

std::random_device Sphere::rs;
std::default_random_engine Sphere::generator(rs());
std::uniform_real_distribution<float> Sphere::distribution(-1e-4f, 1e-4f);

void Sphere::InitVertex() {
    std::vector<glm::vec3> vertices, normals;
    std::vector<glm::vec2> texCoords;

    GLfloat x, y, z;
    GLfloat meriCos;
    GLfloat normCoef = 1.f / BaseRadius;
    GLfloat s, t;

    GLfloat constexpr PI = glm::pi<float>();
    GLfloat paraStep = 2.f * PI / parallelCount;
    GLfloat meriStep = PI / meridianCount;
    GLfloat paraAngle, meriAngle;

    for (int i = 0; i < meridianCount + 1; i++) {
        // Parametrizare dupa meridiane
        meriAngle = PI / 2 - i * meriStep;
        meriCos = BaseRadius * glm::cos(meriAngle);
        z = BaseRadius * glm::sin(meriAngle);

        for (int j = 0; j < parallelCount + 1; j++) {
            // Parametrizare dupa paralele
            paraAngle = j * paraStep;

            // Pozitiile varfului
            x = meriCos * glm::cos(paraAngle);
            y = meriCos * glm::sin(paraAngle);
            glm::vec3 pos = glm::vec3(x, y, z);
            vertices.push_back(pos);

            // Valorile normalei
            normals.push_back(pos * normCoef);

            // Valorile coordonatelor de textura
            s = (float)j / parallelCount;
            t = (float)i / meridianCount;
            texCoords.push_back(glm::vec2(s, t));
        }
    }

    GLuint ind1, ind2;
    for (int i = 0; i < meridianCount; i++) {
        ind1 = i * (parallelCount + 1);
        ind2 = ind1 + parallelCount + 1;

        for (int j = 0; j < parallelCount; j++) {
            if (i != 0) {
                indices.push_back(ind1);
                indices.push_back(ind2);
                indices.push_back(ind1 + 1);
            }

            if (i != meridianCount - 1) {
                indices.push_back(ind1 + 1);
                indices.push_back(ind2);
                indices.push_back(ind2 + 1);
            }

            ind1++;
            ind2++;
        }
    }

    glGenVertexArrays(1, &VaoId);
    glBindVertexArray(VaoId);

    // Incarca pozitiile varfurilor
    glGenBuffers(1, &VboId);
    glBindBuffer(GL_ARRAY_BUFFER, VboId);
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(glm::vec3), &vertices[0], GL_STATIC_DRAW);

    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(glm::vec3), 0);

    // Incarca valorile normalelor
    glGenBuffers(1, &VboNormId);
    glBindBuffer(GL_ARRAY_BUFFER, VboNormId);
    glBufferData(GL_ARRAY_BUFFER, normals.size() * sizeof(glm::vec3), &normals[0], GL_STATIC_DRAW);

    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(glm::vec3), 0);

    // Incarca valorile indicilor
    glGenBuffers(1, &EboId);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EboId);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(GLuint), &indices[0], GL_STATIC_DRAW);
}

void Sphere::DestroyVertex() {
    glDisableVertexAttribArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glDeleteBuffers(1, &VboId);
    glDeleteBuffers(1, &VboNormId);
    glDeleteBuffers(1, &EboId);

    glBindVertexArray(0);
    glDeleteVertexArrays(1, &VaoId);
}

Sphere::Sphere(GLfloat x, GLfloat y, GLfloat z, GLfloat vx, GLfloat vy, GLfloat vz,
    GLfloat r, glm::vec3 col) {
    MatrPos = glm::translate(glm::mat4(1.0f), glm::vec3(x, y, z));
    Velocity = glm::vec3(vx, vy, vz);
    MatrScale = glm::scale(glm::mat4(1.0f), glm::vec3(r, r, r));
    Radius = r * BaseRadius;
    Color = col;
    Force = glm::vec3(distribution(generator), distribution(generator), distribution(generator));
}

GLfloat Sphere::GetRadius() const { return Radius; }

glm::vec3 Sphere::GetCoordinates() const { return glm::vec3(MatrPos[3]); }

glm::vec3 Sphere::GetVelocity() const { return Velocity; }

void Sphere::RenderSphere(GLuint ShaderId) const {
    GLint mModelLoc = glGetUniformLocation(ShaderId, "mModel");
    GLint uColorLoc = glGetUniformLocation(ShaderId, "uColor");

    glm::mat4 mModel = MatrPos * MatrScale;
    glUniformMatrix4fv(mModelLoc, 1, GL_FALSE, &mModel[0][0]);
    glUniform3fv(uColorLoc, 1, &Color[0]);

    glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, 0);
}

void Sphere::Update(bool ChangeForce) {
    if (ChangeForce) {
        Force = glm::vec3(distribution(generator), distribution(generator), distribution(generator));
    }
    Velocity += Force;

    MatrPos[3].x += Velocity.x;
    MatrPos[3].y += Velocity.y;
    MatrPos[3].z += Velocity.z;

    GLfloat leftx = MatrPos[3].x - Radius;
    GLfloat rightx = MatrPos[3].x + Radius;
    if (leftx < -1.0f || rightx > 1.0f) {
        Velocity.x *= -1.0f;
        if (leftx < -1.0f)
            MatrPos[3].x += -1.0f - leftx;
        else
            MatrPos[3].x -= rightx - 1.0f;
    }
    GLfloat downy = MatrPos[3].y - Radius;
    GLfloat upy = MatrPos[3].y + Radius;

    if (downy < -1.0f || upy > 1.0f) {
        Velocity.y *= -1.0f;
        if (downy < -1.0f)
            MatrPos[3].y += -1.0f - downy;
        else
            MatrPos[3].y -= upy - 1.0f;
    }

    GLfloat farz = MatrPos[3].z - Radius;
    GLfloat nearz = MatrPos[3].z + Radius;

    if (farz < -1.0f || nearz > 1.0f) {
        Velocity.z *= -1.0f;
        if (farz < -1.0f)
            MatrPos[3].z += -1.0f - farz;
        else
            MatrPos[3].z -= nearz - 1.0f;
    }
}

void Sphere::ChangeDirection(glm::vec3 diff, glm::vec3 normal) {
    Velocity = Velocity - diff;
    MatrPos[3].x += normal.x * 0.0005f;
    MatrPos[3].y += normal.y * 0.0005f;
    MatrPos[3].z += normal.z * 0.0005f;
}