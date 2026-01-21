#include <GL/glew.h>
#include <GL/freeglut.h>
#include <math.h>

void initGL() {
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluOrtho2D(-1.0, 1.0, -1.0, 1.0);
}

void reshape(int width, int height) {
    if (height == 0) height = 1;
    glViewport(0, 0, width, height);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    float aspect = (float)width / (float)height;
    if (aspect >= 1.0f)
        gluOrtho2D(-aspect, aspect, -1.0, 1.0);
    else
        gluOrtho2D(-1.0, 1.0, -1.0f / aspect, 1.0f / aspect);
    glMatrixMode(GL_MODELVIEW);
}

void drawCircle(float cx, float cy, float r, int segments) {
    glBegin(GL_POLYGON);
    for (int i = 0; i < segments; i++) {
        float angle = 2.0f * 3.14159f * i / segments;
        glVertex2f(cx + r * cos(angle), cy + r * sin(angle));
    }
    glEnd();
}

void display() {
    glClear(GL_COLOR_BUFFER_BIT);
    glLoadIdentity();

    glColor3f(0.8f, 0.5f, 0.2f);
    glBegin(GL_QUADS);
    glVertex2f(-0.3f, -0.5f);
    glVertex2f(0.3f, -0.5f);
    glVertex2f(0.3f, 0.0f);
    glVertex2f(-0.3f, 0.0f);
    glEnd();

    glColor3f(1.0f, 0.0f, 0.0f);
    glBegin(GL_TRIANGLES);
    glVertex2f(-0.4f, 0.0f);
    glVertex2f(0.4f, 0.0f);
    glVertex2f(0.0f, 0.4f);
    glEnd();

    glColor3f(0.4f, 0.2f, 0.0f);
    glBegin(GL_QUADS);
    glVertex2f(0.45f, -0.5f);
    glVertex2f(0.55f, -0.5f);
    glVertex2f(0.55f, -0.2f);
    glVertex2f(0.45f, -0.2f);
    glEnd();

    glColor3f(0.0f, 0.8f, 0.0f);
    drawCircle(0.5f, -0.1f, 0.15f, 60);

    glColor3f(1.0f, 1.0f, 0.0f);
    drawCircle(-0.7f, 0.7f, 0.15f, 60);

    glFlush();
}

int main(int argc, char** argv) {
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_SINGLE | GLUT_RGB);
    glutInitWindowSize(600, 600);
    glutInitWindowPosition(400, 100);
    glutCreateWindow("Simple 2D Scene");
    glewInit();
    initGL();
    glutDisplayFunc(display);
    glutReshapeFunc(reshape);
    glutMainLoop();
    return 0;
}
