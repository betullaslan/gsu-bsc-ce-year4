#include <GL/glut.h>
#include <stdlib.h>

float posX = 0.0f, posY = 0.0f;
float angle = 0.0f;
float scale = 1.0f;
float aspect = 1.0f;

void initGL() {
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
}

void display() {
    glClear(GL_COLOR_BUFFER_BIT);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    glTranslatef(posX, posY, 0.0f);
    glRotatef(angle, 0.0f, 0.0f, 1.0f);
    glScalef(scale, scale, 1.0f);
    glColor3f(0.0f, 1.0f, 0.0f);
    glBegin(GL_QUADS);
    glVertex2f(-0.2f, -0.2f);
    glVertex2f(0.2f, -0.2f);
    glVertex2f(0.2f, 0.2f);
    glVertex2f(-0.2f, 0.2f);
    glEnd();
    glutSwapBuffers();
}

void reshape(int w, int h) {
    if (h == 0) h = 1;
    aspect = (float)w / (float)h; 
    glViewport(0, 0, w, h);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    if (aspect >= 1.0f)
        gluOrtho2D(-aspect, aspect, -1.0, 1.0);
    else
        gluOrtho2D(-1.0, 1.0, -1.0 / aspect, 1.0 / aspect);
    glutPostRedisplay();
}

void keyboard(unsigned char key, int x, int y) {
    switch (key) {
    case 27: exit(0); break;
    case 'w': posY += 0.05f; break;
    case 's': posY -= 0.05f; break;
    case 'a': posX -= 0.05f; break;
    case 'd': posX += 0.05f; break;
    case 'q': angle += 5.0f; break;
    case 'e': angle -= 5.0f; break;
    case '+': scale += 0.05f; break;
    case '-': if (scale > 0.1f) scale -= 0.05f; break;
    }

    if (aspect >= 1.0f) {
        if (posX > aspect) posX = aspect;
        if (posX < -aspect) posX = -aspect;
        if (posY > 1.0f) posY = 1.0f;
        if (posY < -1.0f) posY = -1.0f;
    }
    else {
        if (posX > 1.0f) posX = 1.0f;
        if (posX < -1.0f) posX = -1.0f;
        if (posY > 1.0f / aspect) posY = 1.0f / aspect;
        if (posY < -1.0f / aspect) posY = -1.0f / aspect;
    }

    glutPostRedisplay();
}

int main(int argc, char** argv) {
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB);
    glutInitWindowSize(600, 600);
    glutInitWindowPosition(400, 100);
    glutCreateWindow("Ma Boite");
    initGL();
    glutDisplayFunc(display);
    glutReshapeFunc(reshape);
    glutKeyboardFunc(keyboard);
    glutMainLoop();
    return 0;
}
