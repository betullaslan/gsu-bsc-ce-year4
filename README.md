# Fourth-Year Projects – Computer Engineering @ Galatasaray University

This repository contains selected coursework and term projects from the fall semester of the fourth year (2025–2026) in the Computer Engineering program at Galatasaray University.

Each project is located in its own folder and includes a separate README with details, code files, and implementation explanations.

## Projects Overview

### 1. `meteo-france-temperature-forecasting/`  
**Temperature Forecasting with Machine Learning**  
A comprehensive end-to-end workflow for forecasting hourly surface air temperatures using Meteo-France weather station data with multiple regression and deep learning models.

**Main Files:**
- `01_data_ingestion_and_cleaning.ipynb`
- `02_eda_and_feature_engineering.ipynb`
- `03_global_regressors_evaluation.ipynb`
- `04_station_level_modeling.ipynb`
- `05_station_clustering_pca_kmeans.ipynb`
- `06_lstm_sequence_forecasting.ipynb`

Features:
- Sequential 6-notebook pipeline for temperature forecasting
- Data cleaning, interpolation, and seasonal decomposition
- Global and station-level regression models (Linear, Random Forest)
- PCA-based station clustering with K-Means
- LSTM sequence forecasting with time-series validation
- Complete evaluation metrics (RMSE, R², MAE)

**Course:** Introduction to the Internet of Things

---

### 2. `minecraft-opengl-simulator/`  
**Minecraft-Inspired 3D World Simulator**  
An interactive Minecraft-inspired 3D world with animated characters, dynamic day-night cycles, procedural textures, and first-person camera controls.

**Main Files:**
- `main.cpp`
- `camera.h`, `character.h`, `cube.h`, `lighting.h`

Features:
- 20×20 procedurally textured grass terrain
- Dynamic day-night cycle with sun, moon, and twinkling stars
- Two animated characters (player-controlled and autonomous NPC)
- First-person camera with WASD + mouse look controls
- Phong lighting model with time-based color transitions
- Voxel-style trees and decorative blocks
- Walking animations with limb swinging

**Course:** Computer Graphics

---

### 3. `opengl-graphics-projects/`  
**OpenGL Graphics Projects Collection**  
A series of 12 progressive graphics applications covering 2D/3D rendering, geometric transformations, animations, and interactive user input.

**Main Files:**
- `01_ColoredPointsGrid.cpp` – 10×10 colored point grid
- `02_ZigzagLine.cpp` – Simple zigzag line pattern
- `03_TriangleFanOctagon.cpp` – Regular octagon using triangle fan
- `04_CheckerboardPattern.cpp` – 8×8 checkerboard with quads
- `05_SimpleScene2D.cpp` – 2D scene with house, tree, sun
- `06_3DAxesDisplay.cpp` – 3D coordinate axes visualization
- `07_WireframeCube.cpp` – 3D wireframe cube rendering
- `08_TransformableSquare.cpp` – Interactive transformations (translate, rotate, scale)
- `09_AnimatedWindmill.cpp` – Animated rotating windmill
- `10_BouncingBall.cpp` – Physics-based bouncing ball
- `11_InteractiveDrawing.cpp` – Mouse-based drawing application
- `12_OrbitingPlanet.cpp` – Planetary orbit simulation

Features:
- Progression from basic 2D primitives to 3D transformations
- Keyboard and mouse input handling
- Real-time animations and physics simulations
- Geometric transformations (translation, rotation, scaling)
- Interactive drawing and shape manipulation

**Course:** Computer Graphics

---

### 4. `socket-chat-application/`  
**Python Socket-Based Chat Application**  
A TCP-based client–server chat application supporting multiple concurrent users with direct messaging and broadcast functionality.

**Main Files:**
- `server.py`
- `client.py`

Features:
- Multi-threaded server handling concurrent connections
- Unique username registration system
- Direct peer-to-peer messaging
- Broadcast messaging to all connected clients
- Thread-safe message delivery with per-connection locks
- Real-time console interface with separate I/O threads
- Graceful disconnection handling

**Course:** Distributed Systems and Applications

---

> For more details, refer to the README files inside each project folder.
