#include "Circle.h"

#include <GL/freeglut.h>
#include <glm/gtc/matrix_transform.hpp>

// Initializarea datelor membre statice
GLuint Circle::VaoId;
GLuint Circle::VboId;
GLfloat Circle::BaseRadius = 0.2f;

std::random_device Circle::rs;
std::default_random_engine Circle::generator(rs());
std::uniform_real_distribution<float> Circle::distribution(-1e-4f, 1e-4f);

void Circle::InitVertex() {
    GLfloat Vertices[] = {
        -BaseRadius,  BaseRadius, 0.0f, 1.0f,
         BaseRadius,  BaseRadius, 0.0f, 1.0f,
         BaseRadius, -BaseRadius, 0.0f, 1.0f,
        -BaseRadius, -BaseRadius, 0.0f, 1.0f
    };

    glGenBuffers(1, &VboId);
    glBindBuffer(GL_ARRAY_BUFFER, VboId);
    glBufferData(GL_ARRAY_BUFFER, sizeof(Vertices), Vertices, GL_STATIC_DRAW);

    glGenVertexArrays(1, &VaoId);
    glBindVertexArray(VaoId);

    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 0, 0);
}

void Circle::DestroyVertex() {
    glDisableVertexAttribArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glDeleteBuffers(1, &VboId);
    
    glBindVertexArray(0);
    glDeleteVertexArrays(1, &VaoId);
}

Circle::Circle(GLfloat x, GLfloat y, GLfloat vx, GLfloat vy, GLfloat r, glm::vec3 col) {
    MatrPos = glm::translate(glm::mat4(1.0f), glm::vec3(x, y, 0.f));
    Velocity = glm::vec2(vx, vy);
    MatrScale = glm::scale(glm::mat4(1.0f), glm::vec3(r, r, 0.f));
    Radius = r * BaseRadius;
    Color = col;
    Force = glm::vec2(distribution(generator), distribution(generator));
}

GLfloat Circle::GetRadius() const { return Radius; }

glm::vec2 Circle::GetCoordinates() const { return glm::vec2(MatrPos[3]); }

glm::vec2 Circle::GetVelocity() const { return Velocity; }

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
       
    glDrawArrays(GL_TRIANGLE_FAN, 0, 4);
}

void Circle::Update(bool ChangeForce) {
    if (ChangeForce) {
        Force = glm::vec2(distribution(generator), distribution(generator));
    }
    Velocity += Force;

    MatrPos[3].x += Velocity.x;
    MatrPos[3].y += Velocity.y;

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

void Circle::ChangeDirection(glm::vec2 diff, glm::vec2 normal) {
    Velocity = Velocity - diff;
    MatrPos[3].x += normal.x * 0.005f;
    MatrPos[3].y += normal.y * 0.005f;
}