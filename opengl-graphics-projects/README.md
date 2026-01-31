# OpenGL Graphics Projects Collection

This repository contains a collection of beginner-to-intermediate level OpenGL/C++ graphics applications, covering topics such as 2D/3D rendering, geometric transformations, animations, and interactive user input. Each project is self-contained and demonstrates core computer graphics concepts through visual examples.

---

## Project Info

- **Project Name:** OpenGL Graphics Projects  
- **Language:** C++  
- **Graphics Library:** OpenGL (GLUT/FreeGLUT, GLEW)  
- **Author:** Betul Aslan  

---

## Academic Info

- **Course:** Computer Graphics
- **Institution:** Galatasaray University  
- **Department:** Computer Engineering  
- **Academic Year:** 2025–2026 Fall
- **Assignment:** Course Assignments and Projects  

---

## File: `01_ColoredPointsGrid.cpp`

**Description:**  
A 2D grid of colored points rendered using OpenGL. The program creates a 10x10 grid where each point is assigned a unique color based on its position, demonstrating basic vertex coloring and point rendering techniques.

---

## File: `02_ZigzagLine.cpp`

**Description:**  
A simple 2D zigzag line pattern created using GL_LINE_STRIP. The program draws a blue zigzag line by alternating vertex positions between upper and lower y-coordinates, demonstrating sequential line rendering.

---

## File: `03_TriangleFanOctagon.cpp`

**Description:**  
A regular octagon rendered using GL_TRIANGLE_FAN primitive. The shape is created by calculating vertices in a circular pattern with 8 sides, demonstrating triangle fan rendering technique and trigonometric vertex calculation.

---

## File: `04_CheckerboardPattern.cpp`

**Description:**  
An 8x8 checkerboard pattern rendered using GL_QUADS. The program alternates between black and white squares based on row and column parity, demonstrating quad rendering and conditional coloring logic.

---

## File: `05_SimpleScene2D.cpp`

**Description:**  
A simple 2D scene composition featuring a house with a triangular roof, rectangular body, a tree with circular foliage, and a sun. The scene demonstrates combining multiple primitives (triangles, quads, circles) to create a composite drawing.

---

## File: `06_3DAxesDisplay.cpp`

**Description:**  
A 3D coordinate system visualization displaying the X, Y, and Z axes in red, green, and blue respectively. The program uses perspective projection and gluLookAt for 3D viewing, demonstrating basic 3D rendering setup with depth testing.

---

## File: `07_WireframeCube.cpp`

**Description:**  
A 3D wireframe cube rendered using GL_LINE_LOOP and GL_LINES. The program draws the cube's edges in 3D space using perspective projection, demonstrating how to construct 3D wireframe objects from line primitives.

---

## File: `08_TransformableSquare.cpp`

**Description:**  
An interactive 2D square that can be transformed using keyboard controls. Users can translate (W/A/S/D), rotate (Q/E), and scale (+/-) the square. The program demonstrates real-time geometric transformations and keyboard input handling with boundary constraints.

---

## File: `09_AnimatedWindmill.cpp`

**Description:**  
An animated windmill with four rotating blades. The windmill can be started/paused with spacebar and its rotation speed can be adjusted with +/- keys. The program demonstrates rotation transformations, animation timing, and user interaction.

---

## File: `10_BouncingBall.cpp`

**Description:**  
A red circular ball that bounces within the window boundaries. The ball moves continuously and reverses direction upon hitting window edges. Users can pause/resume the animation with spacebar. The program demonstrates physics-based motion, collision detection, and continuous animation.

---

## File: `11_InteractiveDrawing.cpp`

**Description:**  
An interactive drawing application where users can draw lines, rectangles, and triangles by clicking two points. Press 1 for line mode, 2 for rectangle mode, 3 for triangle mode, and C to clear the canvas. The program demonstrates mouse input handling, shape storage, and dynamic rendering.

---

## File: `12_OrbitingPlanet.cpp`

**Description:**  
A simple solar system simulation featuring a yellow sun at the center and a blue planet orbiting around it. The orbit speed can be adjusted with +/- keys and paused/resumed with spacebar. The program demonstrates circular motion, hierarchical transformations, and idle animation.

---
