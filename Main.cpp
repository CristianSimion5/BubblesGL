#include <windows.h>

#include <GL/glew.h>
#include <GL/freeglut.h>

#include <vector>
#include <chrono>

#include "loadShaders.h"
#include "Circle.h"
#include "Scene.h"
#include "Quaternion.h"

#include <glm/gtc/constants.hpp>

void GLAPIENTRY
MessageCallback(GLenum source,
    GLenum type,
    GLuint id,
    GLenum severity,
    GLsizei length,
    const GLchar* message,
    const void* userParam)
{
    fprintf(stderr, "GL CALLBACK: %s type = 0x%x, severity = 0x%x, message = %s\n",
        (type == GL_DEBUG_TYPE_ERROR ? "** GL ERROR **" : ""),
        type, severity, message);
}

// Deoarece numele era prea lung si se repeta foarte des am definit un alias
typedef std::chrono::steady_clock timer;

GLuint ProgramId;

// Initializeaza scena cu un numar dat de cercuri
Scene scene(20);

void CreateShaders(void) {
    ProgramId = LoadShaders("Shader.vert", "Shader.frag");
}

void DestroyShaders(void) {
    glDeleteProgram(ProgramId);
}

void Initialize(void) {
    glClearColor(1.0f, 1.0f, 1.0f, 0.0f); // culoarea de fond a ecranului

    // During init, enable debug output
    glEnable(GL_DEBUG_OUTPUT);
    glDebugMessageCallback(MessageCallback, 0);

    CreateShaders();

    // Creeaza VBO-urile si VAO-urile necesare pentru desenare
    Circle::InitVertex();
    scene.CreateVertexBuffers();

    // Activeaza testarea in adancime
    glEnable(GL_DEPTH_TEST);
    
    // Activeaza multisample anti-aliasing
    glEnable(GL_MULTISAMPLE);

    glEnable(GL_CULL_FACE);

    // Activeaza transparenta obiectelor
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
}

void RenderFunction(void) {
    // Punctul de timp pentru inceputul desenarii
    timer::time_point t1 = timer::now();
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    
    scene.RenderScene(ProgramId);
    scene.Update();
    glutPostRedisplay();

    glutSwapBuffers();

    // Punctul de timp pentru sfarsitul desenarii
    timer::time_point t2 = timer::now();
    auto d = t2 - t1;

    // Pentru a bloca FPS-ul la 60, se suspenda activitatea timp de 
    // 1000/60 milisecunde - timpul in care s-a desenat cadrul precedent
    if (d.count() * 1e-6f < 1000.0f / 60.0f)
        Sleep((DWORD) (1000.0f/60.0f - d.count() * 1e-6f)); 
}

void Cleanup(void) {
    DestroyShaders();
    // Distruge VBO-urile si VAO-urile folosite la desenarea cercurilor
    Circle::DestroyVertex();
}
#include <iostream>
int main(int argc, char* argv[]) {
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA | GLUT_DEPTH | GLUT_MULTISAMPLE);
    glutInitWindowPosition(100, 100);
    glutInitWindowSize(1000, 1000);
    glutCreateWindow("Bubbles in OpenGL");
    glewInit();

    Initialize();
    /*for (int i = 0; i < Circle::indices.size(); i += 3) {
        std::cout << Circle::indices[i] << ' ' << Circle::indices[i + 1] << ' '
            << Circle::indices[2] << '\n';
    }*/

    glutDisplayFunc(RenderFunction);
    glutCloseFunc(Cleanup);
    glutMainLoop();

    //float constexpr pi = glm::pi<float>();
    //Quaternion quat(glm::normalize(glm::vec3(1.f, 1.f, 0.f)), pi / 2);
    //glm::vec3 a;
    //a = (glm::vec3) (quat * glm::vec3(0.f, 1.f, 0.f) * quat.Conjugate());
    //std::cout << quat << '\n' << a.x << ' ' << a.y << ' ' << a.z;
}