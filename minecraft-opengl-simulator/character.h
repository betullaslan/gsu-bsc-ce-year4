#ifndef CHARACTER_H
#define CHARACTER_H

#include "Cube.h"
#include "Camera.h"
#include <GL/glut.h>
#include <cmath>

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

// Character class built from cubes with animation
class Character {
public:
    Vec3 position;
    float rotation;
    float movementSpeed;
    
    float walkCycle;
    float limbSwing;
    
    Vec3 bodyColor;
    Vec3 skinColor;
    Vec3 limbColor;
    
    Character(Vec3 pos = Vec3(0.0f, 0.5f, 0.0f), Vec3 bColor = Vec3(0.2f, 0.6f, 0.8f))
        : position(pos), rotation(0.0f), movementSpeed(2.0f),
          walkCycle(0.0f), limbSwing(0.0f), 
          bodyColor(bColor), skinColor(Vec3(0.9f, 0.7f, 0.5f)), limbColor(Vec3(0.1f, 0.1f, 0.5f)) {
    }
    
    // Render character using legacy OpenGL
    void drawLegacy() {
        glPushMatrix();
        
        glTranslatef(position.x, position.y, position.z);
        glRotatef(rotation * 180.0f / static_cast<float>(M_PI), 0.0f, 1.0f, 0.0f);
        
        // Body
        glPushMatrix();
        glTranslatef(0.0f, 1.0f, 0.0f);
        glScalef(0.8f, 1.2f, 0.5f);
        glColor3f(bodyColor.x, bodyColor.y, bodyColor.z);
        glutSolidCube(1.0f);
        glPopMatrix();
        
        // Head
        glPushMatrix();
        glTranslatef(0.0f, 2.0f, 0.0f);
        glScalef(0.6f, 0.6f, 0.6f);
        glColor3f(skinColor.x, skinColor.y, skinColor.z);
        glutSolidCube(1.0f);
        glPopMatrix();
        
        // Left leg with animation
        float leftLegSwing = static_cast<float>(sin(limbSwing)) * 0.3f;
        glPushMatrix();
        glTranslatef(-0.25f, 0.3f, 0.0f);
        glRotatef(leftLegSwing * 30.0f, 1.0f, 0.0f, 0.0f);
        glScalef(0.3f, 0.8f, 0.3f);
        glColor3f(limbColor.x, limbColor.y, limbColor.z);
        glutSolidCube(1.0f);
        glPopMatrix();
        
        // Right leg with animation
        float rightLegSwing = static_cast<float>(sin(limbSwing + M_PI)) * 0.3f;
        glPushMatrix();
        glTranslatef(0.25f, 0.3f, 0.0f);
        glRotatef(rightLegSwing * 30.0f, 1.0f, 0.0f, 0.0f);
        glScalef(0.3f, 0.8f, 0.3f);
        glColor3f(limbColor.x, limbColor.y, limbColor.z);
        glutSolidCube(1.0f);
        glPopMatrix();
        
        // Left arm with animation
        float leftArmSwing = static_cast<float>(sin(limbSwing + M_PI)) * 0.4f;
        glPushMatrix();
        glTranslatef(-0.6f, 1.2f, 0.0f);
        glRotatef(leftArmSwing * 25.0f, 1.0f, 0.0f, 0.0f);
        glScalef(0.25f, 0.9f, 0.25f);
        glColor3f(skinColor.x, skinColor.y, skinColor.z);
        glutSolidCube(1.0f);
        glPopMatrix();
        
        // Right arm with animation
        float rightArmSwing = static_cast<float>(sin(limbSwing)) * 0.4f;
        glPushMatrix();
        glTranslatef(0.6f, 1.2f, 0.0f);
        glRotatef(rightArmSwing * 25.0f, 1.0f, 0.0f, 0.0f);
        glScalef(0.25f, 0.9f, 0.25f);
        glColor3f(skinColor.x, skinColor.y, skinColor.z);
        glutSolidCube(1.0f);
        glPopMatrix();
        
        glPopMatrix();
    }
    
    // Process character movement with boundary checking
    void move(int direction, float deltaTime) {
        Vec3 oldPosition = position;
        float velocity = movementSpeed * deltaTime;
        
        switch (direction) {
            case 0:
                position.z -= velocity * static_cast<float>(cos(rotation));
                position.x -= velocity * static_cast<float>(sin(rotation));
                limbSwing += velocity * 5.0f;
                break;
            case 1:
                position.z += velocity * static_cast<float>(cos(rotation));
                position.x += velocity * static_cast<float>(sin(rotation));
                limbSwing += velocity * 5.0f;
                break;
            case 2:
                rotation += velocity;
                break;
            case 3:
                rotation -= velocity;
                break;
        }
        
        // Enforce world boundaries
        const float mapBoundary = 9.0f;
        if (position.x < -mapBoundary || position.x > mapBoundary ||
            position.z < -mapBoundary || position.z > mapBoundary) {
            position = oldPosition;
        }
    }
    
    // Automatic circular movement for NPC
    void autoMove(float deltaTime, float time) {
        float radius = 3.0f;
        position.x = radius * static_cast<float>(cos(time * 0.5));
        position.z = radius * static_cast<float>(sin(time * 0.5));
        rotation = time * 0.5f + static_cast<float>(M_PI / 2.0);
        limbSwing = time * 5.0f;
    }
};

#endif
