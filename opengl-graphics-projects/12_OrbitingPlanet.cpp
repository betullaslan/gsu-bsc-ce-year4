#include <GL/glut.h>
#include <math.h>
#include <stdbool.h>

float angle = 0.0f;
float speed = 0.06f;
bool running = true;

void initGL() {
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
}

void drawCircle(float r, float R, float G, float B) {
    int n = 60;
    float t = 2.0f * 3.1415926f;
    glColor3f(R, G, B);
    glBegin(GL_TRIANGLE_FAN);
    glVertex2f(0.0f, 0.0f);
    for (int i = 0; i <= n; i++)
        glVertex2f(r * cosf(i * t / n), r * sinf(i * t / n));
    glEnd();
}

void display() {
    glClear(GL_COLOR_BUFFER_BIT);
    glLoadIdentity();
    drawCircle(0.2f, 1.0f, 1.0f, 0.0f);
    glPushMatrix();
    glRotatef(angle, 0.0f, 0.0f, 1.0f);
    glTranslatef(0.6f, 0.0f, 0.0f);
    drawCircle(0.06f, 0.0f, 0.0f, 1.0f);
    glPopMatrix();
    glutSwapBuffers();
}

void idle() {
    if (running) {
        angle += speed;
        if (angle > 360.0f) angle -= 360.0f;
        glutPostRedisplay();
    }
}

void keyboard(unsigned char key, int x, int y) {
    if (key == 27) exit(0);
    if (key == 32) running = !running;
    if (key == '+') speed += 0.02f;
    if (key == '-') { speed -= 0.02f; if (speed < 0.0f) speed = 0.0f; }
}

void reshape(int w, int h) {
    if (h == 0) h = 1;
    glViewport(0, 0, w, h);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    float aspect = (float)w / (float)h;
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
    glutCreateWindow("Systeme Solaire");
    initGL();
    glutDisplayFunc(display);
    glutReshapeFunc(reshape);
    glutKeyboardFunc(keyboard);
    glutIdleFunc(idle);
    glutMainLoop();
    return 0;
}
