#include <GL/glut.h>
#include <math.h>
#include <stdbool.h>

float xPos = 0.0f, yPos = 0.0f;
float xSpeed = 0.02f, ySpeed = 0.015f;
float radius = 0.1f;
float xLimit = 1.0f, yLimit = 1.0f;
float aspect = 1.0f;
bool moving = true;

void initGL() {
    glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
}

void drawCircle(float r) {
    int i, n = 50;
    float t = 2.0f * 3.1415926f;
    glBegin(GL_TRIANGLE_FAN);
    glColor3f(1.0f, 0.0f, 0.0f);
    glVertex2f(0.0f, 0.0f);
    for (i = 0; i <= n; i++)
        glVertex2f(r * cosf(i * t / n), r * sinf(i * t / n));
    glEnd();
}

void display() {
    glClear(GL_COLOR_BUFFER_BIT);
    glLoadIdentity();
    glTranslatef(xPos, yPos, 0.0f);
    drawCircle(radius);
    glutSwapBuffers();
}

void timer(int v) {
    if (moving) {
        xPos += xSpeed;
        yPos += ySpeed;
        if (xPos + radius > xLimit || xPos - radius < -xLimit) xSpeed = -xSpeed;
        if (yPos + radius > yLimit || yPos - radius < -yLimit) ySpeed = -ySpeed;
    }
    glutPostRedisplay();
    glutTimerFunc(16, timer, 0);
}

void keyboard(unsigned char key, int x, int y) {
    if (key == 27) exit(0);
    if (key == 32) moving = !moving;
}

void reshape(int w, int h) {
    if (h == 0) h = 1;
    glViewport(0, 0, w, h);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    aspect = (float)w / (float)h;
    if (aspect >= 1.0f) {
        gluOrtho2D(-aspect, aspect, -1.0, 1.0);
        xLimit = aspect;
        yLimit = 1.0f;
    }
    else {
        gluOrtho2D(-1.0, 1.0, -1.0 / aspect, 1.0 / aspect);
        xLimit = 1.0f;
        yLimit = 1.0f / aspect;
    }
    glMatrixMode(GL_MODELVIEW);
}

int main(int argc, char** argv) {
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB);
    glutInitWindowSize(600, 600);
    glutInitWindowPosition(400, 100);
    glutCreateWindow("Bouncing Ball");
    initGL();
    glutDisplayFunc(display);
    glutReshapeFunc(reshape);
    glutKeyboardFunc(keyboard);
    glutTimerFunc(0, timer, 0);
    glutMainLoop();
    return 0;
}
