# Minecraft-Inspired 3D World Simulator

An interactive, cube-based 3D world simulator built with C++ and OpenGL, featuring animated characters, dynamic day-night cycles, procedural textures, and immersive first-person camera controls. This project demonstrates fundamental computer graphics concepts including 3D transformations, lighting systems, texture mapping, and real-time animation.

---

## Project Info

- **Project Name:** Minecraft-Style OpenGL World Simulator
- **Language:** C++
- **Graphics API:** OpenGL (Fixed-Function Pipeline)
- **Windowing Library:** GLUT / FreeGLUT
- **Author:** Betul Aslan  

---

## Academic Info

- **Course:** Computer Graphics
- **Institution:** Galatasaray University  
- **Department:** Computer Engineering  
- **Academic Year:** 2025–2026 Fall
- **Assignment:** Term Project

---

## Key Features

### World Environment
- **Procedural Terrain Generation:** 20×20 block grass terrain with textured cubes
- **Dynamic Lighting System:** Realistic day-night cycle with smooth transitions
- **Sky Rendering:** Color-changing sky that transitions from day blue → sunset orange → night dark blue
- **Celestial Bodies:** Sun and moon with arc-based movement across the sky
- **Night Sky:** 50+ procedurally placed twinkling stars with varying brightness
- **Decorative Elements:** Voxel-style trees with trunk and leaf blocks
- **Random Block Placement:** Scattered blocks for environmental variety

### Interactive Characters
- **Player Character:** Fully controllable cube-based humanoid with:
  - Body, head, arms, and legs
  - Walking animation with limb swinging
  - 360° rotation capability
  - Boundary-constrained movement
  - Color-coded body parts (blue body, skin-tone head/arms, dark blue legs)

- **NPC Character:** Autonomous non-player character with:
  - Circular patrol path
  - Synchronized walking animation
  - Different color scheme (green body)
  - Automatic rotation to face movement direction

### Advanced Camera System
- **First-Person Controls:** Smooth WASD movement with mouse look
- **Adjustable Speed:** Dynamic speed multipliers (Q for 3× fast, E for 0.3× slow)
- **Vertical Movement:** Up/Down arrow keys for elevation control
- **Mouse Look:** Full 360° horizontal rotation with 180° vertical pitch
- **Auto-Recentering:** Cursor repositions when approaching window edges
- **Toggle Mode:** Mouse control can be enabled/disabled with M key

### Dynamic Lighting & Shading
- **Time-Based Lighting:** Light intensity and color change throughout the day
- **Multiple Light Phases:**
  - **Daytime:** Bright white light with high ambient strength
  - **Sunset/Sunrise:** Warm orange tones with medium brightness
  - **Nighttime:** Cool blue moonlight with minimal ambient lighting
- **Ambient, Diffuse, and Specular:** Full Phong lighting model implementation
- **Dynamic Shadows:** Depth-tested rendering for proper object occlusion

### Graphics & Rendering
- **Procedural Texture Generation:** 64×64 grass texture created algorithmically
- **Texture Mapping:** UV-mapped cubes with proper coordinate alignment
- **Smooth Shading:** GL_SMOOTH shading model for realistic surface appearance
- **Color Materials:** Dynamic color assignment based on object properties
- **Depth Testing:** Proper 3D rendering with z-buffer

---

## Complete Control Scheme

### Camera Controls
| Key | Action | Description |
|-----|--------|-------------|
| `W` | Move Forward | Camera moves in the direction it's facing |
| `S` | Move Backward | Camera moves opposite to facing direction |
| `A` | Strafe Left | Camera moves left perpendicular to facing |
| `D` | Strafe Right | Camera moves right perpendicular to facing |
| `↑` | Ascend | Move camera upward (world Y-axis) |
| `↓` | Descend | Move camera downward (world Y-axis) |
| `Mouse` | Look Around | Free-look rotation (when enabled) |
| `Q` | Speed Boost | 3× camera movement speed |
| `E` | Slow Motion | 0.3× camera movement speed |
| `M` | Toggle Mouse | Enable/disable mouse look mode |

### Player Character Controls
| Key | Action | Description |
|-----|--------|-------------|
| `U` | Move Forward | Player moves forward in facing direction |
| `J` | Move Backward | Player moves backward |
| `H` | Turn Left | Rotate player counterclockwise |
| `K` | Turn Right | Rotate player clockwise |

### System Controls
| Key | Action | Description |
|-----|--------|-------------|
| `T` | Cycle Speed | Toggle day-night cycle (1× ↔ 5×) |
| `ESC` | Exit | Quit the application |

---

## Project Structure

```
minecraft-opengl-simulator/
├── main.cpp          # Application entry point and main render loop
├── camera.h          # Camera class and 3D vector/matrix mathematics
├── cube.h            # Cube rendering and transformation utilities
├── character.h       # Character class with animation system
├── lighting.h        # Lighting system and day-night cycle
└── README.md         # This file
```

### File Descriptions

#### `main.cpp`
**Purpose:** Application orchestration and rendering pipeline
- GLUT initialization and window creation (1280×720)
- OpenGL state configuration (depth testing, lighting, materials)
- Main rendering loop with frame timing
- Input handling (keyboard, mouse, special keys)
- Terrain generation and world setup
- Procedural texture creation
- Scene rendering coordination
- 16ms timer-based update cycle

#### `camera.h`
**Purpose:** 3D camera mathematics and control
- `Vec3` structure: 3D vector with operator overloading
- `Mat4` structure: 4×4 transformation matrices
- Perspective projection matrix calculation
- Look-at view matrix generation
- Keyboard movement processing (6 degrees of freedom)
- Mouse-based rotation with yaw/pitch
- Zoom control via field-of-view adjustment
- Vector normalization and cross products

#### `character.h`
**Purpose:** Animated character rendering and behavior
- Cube-based humanoid model construction
- Skeletal animation system with sine-wave limb swing
- Walking cycle synchronization
- Movement with boundary collision detection
- Player input-driven movement
- NPC circular patrol AI
- Color-coded body parts
- Rotation and transformation management

#### `cube.h`
**Purpose:** Cube primitive rendering
- Basic cube geometry generation
- Transformation utilities
- Texture coordinate mapping
- Color application

#### `lighting.h`
**Purpose:** Dynamic lighting and atmospheric effects
- Day-night cycle simulation with configurable speed
- Sun/moon position calculation using trigonometry
- Light color transitions (white → orange → blue)
- Ambient strength modulation
- Sky color computation based on time
- Three distinct lighting phases (day/sunset/night)
- Smooth interpolation between lighting states

---
