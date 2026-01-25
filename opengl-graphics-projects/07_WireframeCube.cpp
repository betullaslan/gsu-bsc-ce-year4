#include <GL/glew.h>
#include <GL/freeglut.h>

void initGL() {
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    glEnable(GL_DEPTH_TEST);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(60.0f, 4.0f / 3.0f, 1.0f, 20.0f);
    glMatrixMode(GL_MODELVIEW);
}

void reshape(int width, int height) {
    if (height == 0) height = 1;
    glViewport(0, 0, width, height);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(60.0f, (float)width / (float)height, 1.0f, 20.0f);
    glMatrixMode(GL_MODELVIEW);
}

void display() {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glLoadIdentity();
    gluLookAt(2.0f, 2.0f, 2.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f);
    glColor3f(1.0f, 1.0f, 1.0f);
    float s = 0.5f;

    glBegin(GL_LINE_LOOP);
    glVertex3f(-s, -s, s);
    glVertex3f(s, -s, s);
    glVertex3f(s, s, s);
    glVertex3f(-s, s, s);
    glEnd();

    glBegin(GL_LINE_LOOP);
    glVertex3f(-s, -s, -s);
    glVertex3f(s, -s, -s);
    glVertex3f(s, s, -s);
    glVertex3f(-s, s, -s);
    glEnd();

    glBegin(GL_LINES);
    glVertex3f(-s, -s, s); glVertex3f(-s, -s, -s);
    glVertex3f(s, -s, s);  glVertex3f(s, -s, -s);
    glVertex3f(s, s, s);   glVertex3f(s, s, -s);
    glVertex3f(-s, s, s);  glVertex3f(-s, s, -s);
    glEnd();

    glutSwapBuffers();
}

int main(int argc, char** argv) {
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
    glutInitWindowSize(600, 600);
    glutInitWindowPosition(400, 100);
    glutCreateWindow("Wireframe Cube");
    glewInit();
    initGL();
    glutDisplayFunc(display);
    glutReshapeFunc(reshape);
    glutMainLoop();
    return 0;
}
