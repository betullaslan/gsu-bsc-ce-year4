// Minecraft Project: A cube-based world with animated characters, day-night cycle, and interactive camera

#include <windows.h>
#include <GL/glut.h>
#include <iostream>
#include <cmath>
#include <vector>
#include <ctime>

#include "Camera.h"
#include "Cube.h"
#include "Character.h"
#include "Lighting.h"

// Window configuration
int windowWidth = 1280;
int windowHeight = 720;

// Camera system
Camera camera(Vec3(0.0f, 3.0f, 10.0f));
int lastMouseX = windowWidth / 2;
int lastMouseY = windowHeight / 2;
bool firstMouse = true;
bool mouseLookEnabled = true;

// Frame timing
float deltaTime = 0.0f;
float lastFrame = 0.0f;

// Input tracking
bool keys[256] = { false };
bool specialKeys[256] = { false };

// Day-night cycle speed multiplier
float dayNightSpeed = 1.0f;

// World configuration
const int groundSize = 20;
std::vector<Vec3> groundPositions;
std::vector<Vec3> blockPositions;

// Scene entities
Character* player = nullptr;
Character* npc = nullptr;
LightingSystem* lighting = nullptr;

// Textures
GLuint grassTexture = 0;

// Night sky stars
std::vector<Vec3> starPositions;
std::vector<float> starBrightness;

// Function declarations
void display();
void reshape(int width, int height);
void keyboard(unsigned char key, int x, int y);
void keyboardUp(unsigned char key, int x, int y);
void specialKey(int key, int x, int y);
void specialKeyUp(int key, int x, int y);
void mouse(int button, int state, int x, int y);
void mouseMotion(int x, int y);
void update(int value);
void init();
void cleanup();
void drawCube(float x, float y, float z, float r, float g, float b);
void processInput();
void toggleMouseLook();
GLuint loadTexture(const char* filename);
void drawTexturedCube(float x, float y, float z);
void drawTree(float x, float z);
void drawStars();

int main(int argc, char** argv)
{
    // Initialize GLUT context
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
    glutInitWindowSize(windowWidth, windowHeight);
    glutInitWindowPosition(50, 50);
    glutCreateWindow("Minecraft");

    init();

    // Register GLUT callbacks
    glutDisplayFunc(display);
    glutReshapeFunc(reshape);
    glutKeyboardFunc(keyboard);
    glutKeyboardUpFunc(keyboardUp);
    glutSpecialFunc(specialKey);
    glutSpecialUpFunc(specialKeyUp);
    glutMouseFunc(mouse);
    glutMotionFunc(mouseMotion);
    glutPassiveMotionFunc(mouseMotion);
    glutTimerFunc(16, update, 0);

    // Display controls and features
    std::cout << "=== MINECRAFT===" << std::endl;
    std::cout << "\nCONTROLS:" << std::endl;
    std::cout << "  WASD       - Camera movement" << std::endl;
    std::cout << "  Arrow Keys - Move up/down" << std::endl;
    std::cout << "  UHJK       - Control player character" << std::endl;
    std::cout << "  Mouse      - Look around" << std::endl;
    std::cout << "  M          - Toggle mouse control" << std::endl;
    std::cout << "  T          - Toggle day-night speed (1x/5x)" << std::endl;
    std::cout << "  Q          - Fast camera movement (3x)" << std::endl;
    std::cout << "  E          - Slow camera movement (0.3x)" << std::endl;
    std::cout << "  ESC        - Exit" << std::endl;
    std::cout << "\nFEATURES:" << std::endl;
    std::cout << "  - Cube-based 20x20 world with procedural grass texture" << std::endl;
    std::cout << "  - Two animated characters (player and NPC)" << std::endl;
    std::cout << "  - Dynamic day-night cycle with sky color transitions" << std::endl;
    std::cout << "  - Twinkling stars visible at night" << std::endl;
    std::cout << "  - Interactive camera with adjustable speed" << std::endl;
    std::cout << "  - Cube-based trees with animated leaves\n" << std::endl;

    glutMainLoop();

    cleanup();
    return 0;
}

void init()
{
    // Configure OpenGL rendering
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_LIGHTING);
    glEnable(GL_LIGHT0);
    glEnable(GL_COLOR_MATERIAL);
    glColorMaterial(GL_FRONT, GL_AMBIENT_AND_DIFFUSE);
    glShadeModel(GL_SMOOTH);
    glEnable(GL_NORMALIZE);
    
    // Generate procedural grass texture
    grassTexture = loadTexture("grass.png");
    
    // Initialize characters
    player = new Character(Vec3(0.0f, 0.0f, 0.0f));
    npc = new Character(Vec3(5.0f, 0.0f, 5.0f), Vec3(0.2f, 0.8f, 0.2f));
    lighting = new LightingSystem();
    
    // Generate ground grid
    for (int x = 0; x < groundSize; x++) {
        for (int z = 0; z < groundSize; z++) {
            groundPositions.push_back(Vec3(
                x - groundSize / 2.0f,
                -0.5f,
                z - groundSize / 2.0f
            ));
        }
    }
    
    // Place random blocks
    blockPositions.push_back(Vec3(3.0f, 0.5f, 3.0f));
    blockPositions.push_back(Vec3(-3.0f, 0.5f, 3.0f));
    blockPositions.push_back(Vec3(3.0f, 0.5f, -3.0f));
    blockPositions.push_back(Vec3(-3.0f, 0.5f, -3.0f));
    blockPositions.push_back(Vec3(5.0f, 0.5f, 0.0f));
    blockPositions.push_back(Vec3(-5.0f, 0.5f, 0.0f));
    
    // Generate stars for night sky
    srand(static_cast<unsigned int>(time(0)));
    for (int i = 0; i < 50; i++) {
        float x = (rand() % 200 - 100) / 10.0f;
        float y = 20.0f + (rand() % 100) / 10.0f;
        float z = (rand() % 200 - 100) / 10.0f;
        starPositions.push_back(Vec3(x, y, z));
        starBrightness.push_back(0.5f + (rand() % 100) / 200.0f);
    }
    
    toggleMouseLook();
}

void cleanup()
{
    delete player;
    delete npc;
    delete lighting;
}

void toggleMouseLook()
{
    mouseLookEnabled = !mouseLookEnabled;
    
    if (mouseLookEnabled) {
        glutSetCursor(GLUT_CURSOR_NONE);
        glutWarpPointer(windowWidth / 2, windowHeight / 2);
        lastMouseX = windowWidth / 2;
        lastMouseY = windowHeight / 2;
        firstMouse = true;
        std::cout << "Mouse control: ENABLED" << std::endl;
    } else {
        glutSetCursor(GLUT_CURSOR_INHERIT);
        std::cout << "Mouse control: DISABLED" << std::endl;
    }
}

void display()
{
    // Set sky color based on time of day
    Vec3 skyColor = lighting->getSkyColor();
    glClearColor(skyColor.x, skyColor.y, skyColor.z, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    
    // Configure camera view
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    
    Vec3 target = camera.position + camera.front;
    gluLookAt(
        camera.position.x, camera.position.y, camera.position.z,
        target.x, target.y, target.z,
        camera.up.x, camera.up.y, camera.up.z
    );
    
    // Configure dynamic lighting
    Vec3 lightPos = lighting->lightPosition;
    Vec3 lightColor = lighting->lightColor;
    float ambient = lighting->ambientStrength;
    
    GLfloat light_position[] = { lightPos.x, lightPos.y, lightPos.z, 1.0f };
    GLfloat light_ambient[] = { lightColor.x * ambient, lightColor.y * ambient, lightColor.z * ambient, 1.0f };
    GLfloat light_diffuse[] = { lightColor.x, lightColor.y, lightColor.z, 1.0f };
    GLfloat light_specular[] = { 0.5f, 0.5f, 0.5f, 1.0f };
    
    glLightfv(GL_LIGHT0, GL_POSITION, light_position);
    glLightfv(GL_LIGHT0, GL_AMBIENT, light_ambient);
    glLightfv(GL_LIGHT0, GL_DIFFUSE, light_diffuse);
    glLightfv(GL_LIGHT0, GL_SPECULAR, light_specular);
    
    // Render ground with texture
    for (size_t i = 0; i < groundPositions.size(); i++) {
        drawTexturedCube(groundPositions[i].x, groundPositions[i].y, groundPositions[i].z);
    }
    
    // Render blocks with varying colors
    for (size_t i = 0; i < blockPositions.size(); i++) {
        if (i % 2 == 0) {
            drawCube(blockPositions[i].x, blockPositions[i].y, blockPositions[i].z, 0.6f, 0.4f, 0.2f);
        } else {
            drawCube(blockPositions[i].x, blockPositions[i].y, blockPositions[i].z, 0.5f, 0.5f, 0.5f);
        }
    }
    
    // Render trees
    drawTree(-7.0f, -7.0f);
    drawTree(7.0f, 7.0f);
    
    // Render stars at night
    drawStars();
    
    // Render characters
    player->drawLegacy();
    npc->drawLegacy();
    
    glutSwapBuffers();
}

void drawCube(float x, float y, float z, float r, float g, float b)
{
    glPushMatrix();
    glTranslatef(x, y, z);
    glColor3f(r, g, b);
    glutSolidCube(1.0f);
    glPopMatrix();
}

void reshape(int width, int height)
{
    windowWidth = width;
    windowHeight = height;
    
    glViewport(0, 0, width, height);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(camera.zoom, (float)width / (float)height, 0.1f, 100.0f);
    glMatrixMode(GL_MODELVIEW);
}

void keyboard(unsigned char key, int x, int y)
{
    keys[key] = true;
    
    if (key == 27) {
        cleanup();
        exit(0);
    }
    
    if (key == 'm' || key == 'M') {
        toggleMouseLook();
    }
    
    if (key == 't' || key == 'T') {
        dayNightSpeed = (dayNightSpeed == 1.0f) ? 5.0f : 1.0f;
        std::cout << "Day-night cycle: " << (dayNightSpeed == 5.0f ? "FAST (5x)" : "NORMAL") << std::endl;
    }
}

void keyboardUp(unsigned char key, int x, int y)
{
    keys[key] = false;
}

void specialKey(int key, int x, int y)
{
    specialKeys[key] = true;
}

void specialKeyUp(int key, int x, int y)
{
    specialKeys[key] = false;
}

void mouse(int button, int state, int x, int y)
{
    (void)button;
    (void)state;
    (void)x;
    (void)y;
}

void mouseMotion(int x, int y)
{
    if (!mouseLookEnabled) {
        return;
    }
    
    if (firstMouse) {
        lastMouseX = x;
        lastMouseY = y;
        firstMouse = false;
        return;
    }
    
    float xoffset = static_cast<float>(x - lastMouseX);
    float yoffset = static_cast<float>(lastMouseY - y);
    
    lastMouseX = x;
    lastMouseY = y;
    
    camera.processMouseMovement(xoffset, yoffset);
    
    // Recenter cursor when approaching window edges
    if (x < windowWidth / 4 || x > 3 * windowWidth / 4 ||
        y < windowHeight / 4 || y > 3 * windowHeight / 4) {
        glutWarpPointer(windowWidth / 2, windowHeight / 2);
        lastMouseX = windowWidth / 2;
        lastMouseY = windowHeight / 2;
        firstMouse = true;
    }
}

void processInput()
{
    // Camera speed modifiers
    float speedMultiplier = 1.0f;
    if (keys['q'] || keys['Q']) {
        speedMultiplier = 3.0f;
    } else if (keys['e'] || keys['E']) {
        speedMultiplier = 0.3f;
    }
    
    float adjustedDelta = deltaTime * speedMultiplier;
    
    // Camera movement
    if (keys['w'] || keys['W'])
        camera.processKeyboard(0, adjustedDelta);
    if (keys['s'] || keys['S'])
        camera.processKeyboard(1, adjustedDelta);
    if (keys['a'] || keys['A'])
        camera.processKeyboard(2, adjustedDelta);
    if (keys['d'] || keys['D'])
        camera.processKeyboard(3, adjustedDelta);
    
    if (specialKeys[GLUT_KEY_UP])
        camera.processKeyboard(4, adjustedDelta);
    if (specialKeys[GLUT_KEY_DOWN])
        camera.processKeyboard(5, adjustedDelta);
    
    // Player character control
    if (keys['u'] || keys['U'])
        player->move(0, deltaTime);
    if (keys['j'] || keys['J'])
        player->move(1, deltaTime);
    if (keys['h'] || keys['H'])
        player->move(2, deltaTime);
    if (keys['k'] || keys['K'])
        player->move(3, deltaTime);
}

void update(int value)
{
    // Calculate frame delta time
    int currentTime = glutGet(GLUT_ELAPSED_TIME);
    float currentFrame = static_cast<float>(currentTime) / 1000.0f;
    deltaTime = currentFrame - lastFrame;
    lastFrame = currentFrame;
    
    processInput();
    
    // Update lighting system
    lighting->update(deltaTime * dayNightSpeed);
    
    // Animate NPC
    npc->autoMove(deltaTime, currentFrame);
    
    glutPostRedisplay();
    glutTimerFunc(16, update, 0);
}

GLuint loadTexture(const char* filename)
{
    GLuint textureID = 0;
    
    // Generate procedural grass texture
    const int texWidth = 64;
    const int texHeight = 64;
    unsigned char texData[texWidth * texHeight * 3];
    
    for (int y = 0; y < texHeight; y++) {
        for (int x = 0; x < texWidth; x++) {
            int idx = (y * texWidth + x) * 3;
            int variant = (x + y) % 16;
            texData[idx + 0] = 20 + variant;
            texData[idx + 1] = 100 + variant * 5;
            texData[idx + 2] = 20 + variant;
        }
    }
    
    glGenTextures(1, &textureID);
    glBindTexture(GL_TEXTURE_2D, textureID);
    
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, texWidth, texHeight, 0, GL_RGB, GL_UNSIGNED_BYTE, texData);
    
    return textureID;
}

void drawTexturedCube(float x, float y, float z)
{
    glPushMatrix();
    glTranslatef(x, y, z);
    
    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, grassTexture);
    glColor3f(1.0f, 1.0f, 1.0f);
    
    float size = 0.5f;
    
    glBegin(GL_QUADS);
    
    // Top face
    glTexCoord2f(0.0f, 0.0f); glVertex3f(-size, size, -size);
    glTexCoord2f(1.0f, 0.0f); glVertex3f( size, size, -size);
    glTexCoord2f(1.0f, 1.0f); glVertex3f( size, size,  size);
    glTexCoord2f(0.0f, 1.0f); glVertex3f(-size, size,  size);
    
    // Bottom face
    glTexCoord2f(0.0f, 0.0f); glVertex3f(-size, -size, -size);
    glTexCoord2f(1.0f, 0.0f); glVertex3f( size, -size, -size);
    glTexCoord2f(1.0f, 1.0f); glVertex3f( size, -size,  size);
    glTexCoord2f(0.0f, 1.0f); glVertex3f(-size, -size,  size);
    
    // Front face
    glTexCoord2f(0.0f, 0.0f); glVertex3f(-size, -size,  size);
    glTexCoord2f(1.0f, 0.0f); glVertex3f( size, -size,  size);
    glTexCoord2f(1.0f, 1.0f); glVertex3f( size,  size,  size);
    glTexCoord2f(0.0f, 1.0f); glVertex3f(-size,  size,  size);
    
    // Back face
    glTexCoord2f(0.0f, 0.0f); glVertex3f(-size, -size, -size);
    glTexCoord2f(1.0f, 0.0f); glVertex3f( size, -size, -size);
    glTexCoord2f(1.0f, 1.0f); glVertex3f( size,  size, -size);
    glTexCoord2f(0.0f, 1.0f); glVertex3f(-size,  size, -size);
    
    // Left face
    glTexCoord2f(0.0f, 0.0f); glVertex3f(-size, -size, -size);
    glTexCoord2f(1.0f, 0.0f); glVertex3f(-size, -size,  size);
    glTexCoord2f(1.0f, 1.0f); glVertex3f(-size,  size,  size);
    glTexCoord2f(0.0f, 1.0f); glVertex3f(-size,  size, -size);
    
    // Right face
    glTexCoord2f(0.0f, 0.0f); glVertex3f( size, -size, -size);
    glTexCoord2f(1.0f, 0.0f); glVertex3f( size, -size,  size);
    glTexCoord2f(1.0f, 1.0f); glVertex3f( size,  size,  size);
    glTexCoord2f(0.0f, 1.0f); glVertex3f( size,  size, -size);
    
    glEnd();
    
    glDisable(GL_TEXTURE_2D);
    glPopMatrix();
}

void drawTree(float x, float z)
{
    // Tree trunk
    drawCube(x, 0.5f, z, 0.4f, 0.25f, 0.1f);
    drawCube(x, 1.5f, z, 0.4f, 0.25f, 0.1f);
    drawCube(x, 2.5f, z, 0.4f, 0.25f, 0.1f);
    
    // Tree leaves
    for (int dx = -1; dx <= 1; dx++) {
        for (int dy = 0; dy <= 2; dy++) {
            for (int dz = -1; dz <= 1; dz++) {
                if (dy == 0 && (abs(dx) + abs(dz) > 2)) continue;
                drawCube(x + dx, 3.0f + dy, z + dz, 0.1f, 0.6f, 0.1f);
            }
        }
    }
}

void drawStars()
{
    // Only render stars at night
    if (lighting->ambientStrength > 0.4f) {
        return;
    }
    
    // Calculate twinkling animation
    float twinkle = static_cast<float>(sin(glutGet(GLUT_ELAPSED_TIME) / 200.0)) * 0.2f + 0.8f;
    
    glDisable(GL_LIGHTING);
    
    for (size_t i = 0; i < starPositions.size(); i++) {
        float brightness = starBrightness[i] * twinkle;
        
        // Variation in twinkling pattern
        if (i % 3 == 0) {
            brightness *= static_cast<float>(sin(glutGet(GLUT_ELAPSED_TIME) / 300.0 + i)) * 0.3f + 0.7f;
        }
        
        glPushMatrix();
        glTranslatef(starPositions[i].x, starPositions[i].y, starPositions[i].z);
        glColor3f(brightness, brightness, brightness * 0.95f);
        glutSolidCube(0.1f);
        glPopMatrix();
    }
    
    glEnable(GL_LIGHTING);
}
