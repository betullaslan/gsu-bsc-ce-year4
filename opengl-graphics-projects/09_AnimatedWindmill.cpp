#include <GL/glut.h>
#include <stdlib.h>
#include <stdbool.h>

float angle = 0.0f;
float speed = 2.0f;
bool spinning = true;

void initGL() {
    glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
}

void drawPale() {
    glBegin(GL_TRIANGLES);
    glColor3f(0.2f, 0.6f, 1.0f);
    glVertex2f(0.0f, 0.0f);
    glVertex2f(0.1f, 0.5f);
    glVertex2f(-0.1f, 0.5f);
    glEnd();
}

void display() {
    glClear(GL_COLOR_BUFFER_BIT);
    glLoadIdentity();

    glColor3f(0.4f, 0.2f, 0.0f);
    glBegin(GL_QUADS);
    glVertex2f(-0.05f, -0.5f);
    glVertex2f(0.05f, -0.5f);
    glVertex2f(0.05f, 0.0f);
    glVertex2f(-0.05f, 0.0f);
    glEnd();

    glPushMatrix();
    glRotatef(angle, 0.0f, 0.0f, 1.0f);
    for (int i = 0; i < 4; i++) {
        drawPale();
        glRotatef(90.0f, 0.0f, 0.0f, 1.0f);
    }
    glPopMatrix();

    glutSwapBuffers();
}

void timer(int value) {
    if (spinning) {
        angle += speed;
        if (angle > 360.0f) angle -= 360.0f;
    }
    glutPostRedisplay();
    glutTimerFunc(16, timer, 0);
}

void keyboard(unsigned char key, int x, int y) {
    if (key == 27) exit(0);
    if (key == 32) spinning = !spinning;
    if (key == '+') speed += 0.5f;
    if (key == '-') {
        speed -= 0.5f;
        if (speed < 0.1f) speed = 0.1f;
    }
}

void reshape(int w, int h) {
    if (h == 0) h = 1;
    float aspect = (float)w / (float)h;
    glViewport(0, 0, w, h);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    if (aspect >= 1.0f)
        gluOrtho2D(-aspect, aspect, -1.0, 1.0);
    else
        gluOrtho2D(-1.0, 1.0, -1.0 / aspect, 1.0 / aspect);
    glMatrixMode(GL_MODELVIEW);
}

int main(int argc, char** argv) {
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB);
    glutInitWindowSize(600, 600);
    glutInitWindowPosition(400, 100);
    glutCreateWindow("Moulin");
    initGL();
    glutDisplayFunc(display);
    glutReshapeFunc(reshape);
    glutKeyboardFunc(keyboard);
    glutTimerFunc(0, timer, 0);
    glutMainLoop();
    return 0;
}
