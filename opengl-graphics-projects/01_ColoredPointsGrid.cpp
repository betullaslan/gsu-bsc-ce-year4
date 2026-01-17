#include <GL/glew.h>
#include <GL/freeglut.h>

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
    gluOrtho2D(-1.0, 1.0, -1.0, 1.0);
}

void display() {
    glClear(GL_COLOR_BUFFER_BIT);
    glLoadIdentity();
    glPointSize(6.0f);
    glBegin(GL_POINTS);

    int rows = 10, cols = 10;
    float spacing = 0.18f;
    float startX = -((cols - 1) * spacing) / 2.0f;
    float startY = -((rows - 1) * spacing) / 2.0f;

    for (int i = 0; i < rows; i++) {
        for (int j = 0; j < cols; j++) {
            float x = startX + j * spacing;
            float y = startY + i * spacing;
            float r = (float)j / cols;
            float g = (float)i / rows;
            float b = 1.0f - ((float)i + (float)j) / (rows + cols);
            glColor3f(r, g, b);
            glVertex2f(x, y);
        }
    }

    glEnd();
    glutSwapBuffers();
}

int main(int argc, char** argv) {
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB);
    glutInitWindowSize(600, 600);
    glutInitWindowPosition(400, 100);
    glutCreateWindow("2D Points Grid");
    glewInit();
    initGL();
    glutDisplayFunc(display);
    glutReshapeFunc(reshape);
    glutMainLoop();
    return 0;
}
