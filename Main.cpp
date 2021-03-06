#include <windows.h>

#include <GL/glew.h>
#include <GL/freeglut.h>

#include <chrono>

#include "loadShaders.h"
#include "Scene.h"

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

    // Mesaje de debugging
    // glEnable(GL_DEBUG_OUTPUT);
    // glDebugMessageCallback(MessageCallback, 0);

    CreateShaders();

    // Creeaza VBO-urile si VAO-urile necesare pentru desenare
    Sphere::InitVertex();
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

    // Trebuie curatat si bitul buffer-ului de adancime
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
        Sleep((DWORD)(1000.0f / 60.0f - d.count() * 1e-6f));
}

void OnMouseClickEvent(int button, int state, int x, int y) {
    // Verificarea apasarii butoanelor mouse-ului pentru modificarea camerei
    if (button == GLUT_LEFT_BUTTON) {
        if (state == GLUT_UP)
            scene.DisableMouseDrag();
        else {
            scene.EnableMouseDrag();
            scene.SetMouseP1(x, y);
            scene.SetMouseP2(x, y);
        }
    }
    // 3 - Scroll Up, 4 - Scroll Down
    else if (button == 3) {
        scene.Scroll(true);
    }
    else if (button == 4) {
        scene.Scroll(false);
    }
}

void OnMouseMotionEvent(int x, int y) {
    scene.SetMouseP2(x, y);
}

void OnReshapeEvent(int width, int height) {
    glViewport(0, 0, width, height);
    scene.SetScreenSize(width, height);
}

void Cleanup(void) {
    DestroyShaders();
    // Distruge VBO-urile si VAO-urile folosite la desenarea sferelor
    Sphere::DestroyVertex();
}

int main(int argc, char* argv[]) {
    glutInit(&argc, argv);
    // GLUT_DOUBLE - double buffering
    // GLUT_DEPTH - necesar pentru depth testing
    // GLUT_MULTISAMPLE - multisampling avem nevoie de
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA | GLUT_DEPTH | GLUT_MULTISAMPLE);
    glutInitWindowPosition(100, 100);
    glutInitWindowSize(1000, 1000);
    glutCreateWindow("Bubbles in OpenGL");
    glewInit();

    Initialize();

    glutDisplayFunc(RenderFunction);
    glutCloseFunc(Cleanup);
    glutMouseFunc(OnMouseClickEvent);
    glutMotionFunc(OnMouseMotionEvent);
    glutReshapeFunc(OnReshapeEvent);
    glutMainLoop();
}