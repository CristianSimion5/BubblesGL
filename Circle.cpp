#include "Circle.h"

#include <GL/freeglut.h>
#include <glm/gtc/matrix_transform.hpp>

// Initializarea datelor membre statice
GLuint Circle::VaoId;
GLuint Circle::VboId;
GLuint Circle::VboNormId;
GLuint Circle::EboId;
std::vector<GLuint> Circle::indices;

GLfloat Circle::BaseRadius = 0.2f;
GLint Circle::parallelCount = 50;
GLint Circle::meridianCount = 50;

std::random_device Circle::rs;
std::default_random_engine Circle::generator(rs());
std::uniform_real_distribution<float> Circle::distribution(-1e-4f, 1e-4f);

void Circle::InitVertex() {
    std::vector<glm::vec3> vertices, normals;
    std::vector<glm::vec2> texCoords;

    GLfloat x, y, z;
    GLfloat paraCos;
    GLfloat normCoef = 1.f / BaseRadius;
    GLfloat s, t;

    GLfloat constexpr PI = glm::pi<float>();
    GLfloat paraStep = 2.f * PI / parallelCount;
    GLfloat meriStep = 2.f * PI / meridianCount;
    GLfloat paraAngle, meriAngle;

    int II = 1;

    for (int i = 0; i < parallelCount + II; i++) {
        // Parametrizare dupa paralele
        paraAngle = PI / 2 - i * paraStep;
        paraCos = BaseRadius * glm::cos(paraAngle);
        z = BaseRadius * glm::sin(paraAngle);
    
        for (int j = 0; j < meridianCount + II; j++) {
            // Parametrizare dupa meridiane
            meriAngle = j * meriStep;

            // Pozitiile varfului
            x = paraCos * glm::cos(meriAngle);
            y = paraCos * glm::sin(meriAngle);
            glm::vec3 pos = glm::vec3(x, y, z);
            vertices.push_back(pos);

            // Valorile normalei
            normals.push_back(pos * normCoef);

            // Valorile coordonatelor de textura
            s = (float) j / meridianCount;
            t = (float) i / parallelCount;
            texCoords.push_back(glm::vec2(s, t));
        }
    }

    GLuint ind1, ind2;
    for (int i = 0; i < parallelCount; i++) {
        ind1 = i * (meridianCount + II);
        ind2 = ind1 + meridianCount + II;

        for (int j = 0; j < meridianCount; j++) {
            if (i != 0) {
                indices.push_back(ind1);
                indices.push_back(ind2);
                indices.push_back(ind1 + 1);
            }

            if (i != parallelCount - 1) {
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

void Circle::DestroyVertex() {
    glDisableVertexAttribArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glDeleteBuffers(1, &VboId);
    
    glBindVertexArray(0);
    glDeleteVertexArrays(1, &VaoId);
}

Circle::Circle(GLfloat x, GLfloat y, GLfloat z, GLfloat vx, GLfloat vy, GLfloat vz, 
        GLfloat r, glm::vec3 col) {
    MatrPos = glm::translate(glm::mat4(1.0f), glm::vec3(x, y, z));
    Velocity = glm::vec3(vx, vy, vz);
    MatrScale = glm::scale(glm::mat4(1.0f), glm::vec3(r, r, r));
    Radius = r * BaseRadius;
    Color = col;
    Force = glm::vec3(distribution(generator), distribution(generator), distribution(generator));
}

GLfloat Circle::GetRadius() const { return Radius; }

glm::vec3 Circle::GetCoordinates() const { return glm::vec3(MatrPos[3]); }

glm::vec3 Circle::GetVelocity() const { return Velocity; }

void Circle::RenderCircle(GLuint ShaderId) const {
    glUseProgram(ShaderId);

    GLint mScaleLoc = glGetUniformLocation(ShaderId, "mScale");
    GLint mTranslLoc = glGetUniformLocation(ShaderId, "mTranslate");

    glUniformMatrix4fv(mScaleLoc, 1, GL_FALSE, &MatrScale[0][0]);
    glUniformMatrix4fv(mTranslLoc, 1, GL_FALSE, &MatrPos[0][0]);

    GLint uWidthLoc = glGetUniformLocation(ShaderId, "uWidth");
    GLint uHeightLoc = glGetUniformLocation(ShaderId, "uHeight");
    GLint uRadiusLoc = glGetUniformLocation(ShaderId, "radius");
    GLint uColorLoc = glGetUniformLocation(ShaderId, "uColor");

    glUniform1f(uWidthLoc, (float) glutGet(GLUT_WINDOW_WIDTH));
    glUniform1f(uHeightLoc, (float) glutGet(GLUT_WINDOW_HEIGHT));
    glUniform1f(uRadiusLoc, Radius);
    glUniform3fv(uColorLoc, 1, &Color[0]);
       
    glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, 0);
}

void Circle::Update(bool ChangeForce) {
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
}

void Circle::ChangeDirection(glm::vec3 diff, glm::vec3 normal) {
    Velocity = Velocity - diff;
    MatrPos[3].x += normal.x * 0.005f;
    MatrPos[3].y += normal.y * 0.005f;
    MatrPos[3].z += normal.z * 0.005f;
}