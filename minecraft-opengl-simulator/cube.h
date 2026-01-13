#ifndef CUBE_H
#define CUBE_H

#include <GL/glut.h>

// Cube rendering class for block-based world
class Cube {
public:
    Cube() {}
    ~Cube() {}
    
    void draw() const {
        glutSolidCube(1.0f);
    }
    
    void drawAt(float x, float y, float z, float r, float g, float b) const {
        glPushMatrix();
        glTranslatef(x, y, z);
        glColor3f(r, g, b);
        glutSolidCube(1.0f);
        glPopMatrix();
    }
    
    void drawScaled(float x, float y, float z, float sx, float sy, float sz, float r, float g, float b) const {
        glPushMatrix();
        glTranslatef(x, y, z);
        glScalef(sx, sy, sz);
        glColor3f(r, g, b);
        glutSolidCube(1.0f);
        glPopMatrix();
    }
};

// Helper functions for OpenGL transformations
namespace GLTransform {
    inline void translate(float x, float y, float z) {
        glTranslatef(x, y, z);
    }
    
    inline void scale(float x, float y, float z) {
        glScalef(x, y, z);
    }
    
    inline void rotateY(float angle) {
        glRotatef(angle * 180.0f / 3.14159f, 0.0f, 1.0f, 0.0f);
    }
    
    inline void rotateX(float angle) {
        glRotatef(angle * 180.0f / 3.14159f, 1.0f, 0.0f, 0.0f);
    }
    
    inline void rotateZ(float angle) {
        glRotatef(angle * 180.0f / 3.14159f, 0.0f, 0.0f, 1.0f);
    }
    
    inline void setColor(float r, float g, float b) {
        glColor3f(r, g, b);
    }
    
    inline void pushMatrix() {
        glPushMatrix();
    }
    
    inline void popMatrix() {
        glPopMatrix();
    }
}

#endif
