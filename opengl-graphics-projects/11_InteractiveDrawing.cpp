#include <GL/glut.h>
#include <vector>

struct Shape { int type; float x1, y1, x2, y2; };

std::vector<Shape> shapes;
int currentType = 0;
bool firstClick = true;
float startX, startY;

float px(float v) { return v + 0.5f; }
float py(float v) { return v + 0.5f; }

void initGL() {
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glEnable(GL_LINE_SMOOTH);
    glHint(GL_LINE_SMOOTH_HINT, GL_NICEST);
}

void drawShape(const Shape& s) {
    glLineWidth(2.0f);
    if (s.type == 1) {
        glBegin(GL_LINES);
        glVertex2f(px(s.x1), py(s.y1));
        glVertex2f(px(s.x2), py(s.y2));
        glEnd();
    }
    else if (s.type == 2) {
        glBegin(GL_LINE_LOOP);
        glVertex2f(px(s.x1), py(s.y1));
        glVertex2f(px(s.x2), py(s.y1));
        glVertex2f(px(s.x2), py(s.y2));
        glVertex2f(px(s.x1), py(s.y2));
        glEnd();
    }
    else if (s.type == 3) {
        glBegin(GL_LINE_LOOP);
        glVertex2f(px((s.x1 + s.x2) * 0.5f), py(s.y2));
        glVertex2f(px(s.x1), py(s.y1));
        glVertex2f(px(s.x2), py(s.y1));
        glEnd();
    }
}

void display() {
    glClear(GL_COLOR_BUFFER_BIT);
    glColor3f(0.85f, 0.85f, 0.85f);
    for (auto& s : shapes) drawShape(s);
    glutSwapBuffers();
}

void reshape(int w, int h) {
    if (h == 0) h = 1;
    glViewport(0, 0, w, h);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluOrtho2D(0, w, 0, h);
    glMatrixMode(GL_MODELVIEW);
}

void mouse(int button, int state, int x, int y) {
    if (button == GLUT_LEFT_BUTTON && state == GLUT_DOWN && currentType != 0) {
        float fx = (float)x;
        float fy = (float)glutGet(GLUT_WINDOW_HEIGHT) - y;
        if (firstClick) { startX = fx; startY = fy; firstClick = false; }
        else {
            Shape s; s.type = currentType; s.x1 = startX; s.y1 = startY; s.x2 = fx; s.y2 = fy;
            shapes.push_back(s); firstClick = true;
        }
        glutPostRedisplay();
    }
}

void keyboard(unsigned char key, int x, int y) {
    if (key == 27) exit(0);
    if (key == '1') currentType = 1;
    if (key == '2') currentType = 2;
    if (key == '3') currentType = 3;
    if (key == 'c' || key == 'C') { shapes.clear(); currentType = 0; firstClick = true; } 
    glutPostRedisplay();
}

int main(int argc, char** argv) {
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_MULTISAMPLE);
    glutInitWindowSize(900, 800);
    glutInitWindowPosition(200, 0);
    glutCreateWindow("Interactive Drawing");
    initGL();
    glutDisplayFunc(display);
    glutReshapeFunc(reshape);
    glutKeyboardFunc(keyboard);
    glutMouseFunc(mouse);
    glutMainLoop();
    return 0;
}
