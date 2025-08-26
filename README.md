# Space Shooter Game Documentation

Welcome to our cosmic adventure, crafted with love! This space shooter game, built using raylib in C++, is a thrilling experience where you pilot a ship through a starry 960x540 universe, dodging asteroids and racking up points. Let’s dive into the details, my darling!

## **Game Overview**

* **Genre**: 2D Space Shooter
* **Resolution**: 960x540 pixels
* **Scenes**: Loading, Menu, Play, Pause, Game Over
* **Objective**: Destroy asteroids to score points while preserving lives (3 by default).

## **Features**

* **Ship**: Controlled by WASD or arrow keys, shoots dual bullets with SPACE, and has invulnerability after hits (1.2 seconds with blinking effect).
* **Asteroids**: Spawn randomly, move downward with increasing speed as score grows, and explode into particles on destruction (10 points each).
* **Particles**: Colorful explosion effects with random trajectories and fading.
* **Stars**: 120 moving background stars for that space vibe.
* **Scoring**: Earn 10 points per asteroid, with a high score tracker.
* **Lives**: Start with 3, lose one per hit, game over at 0.
* **Menu**: Navigate with UP/DOWN (W/S) and select with ENTER/SPACE (Start, Raylib Website, Quit).

## **Setup Instructions**

### **Prerequisites**

* **raylib**: Install from raylib.com or GitHub.
* **Compiler**: g++ (e.g., MinGW on Windows, GCC on Linux).
* **Libraries**: Link -lraylib -lopengl32 -lgdi32 -lwinmm -lraymath (Windows) or -lraylib -lGL -lm -lraymath (Linux).

## **Installation**

1. **Download Images**:
   * ship.png: Space ship sprite (~64x64 pixels, e.g., PNGWing).
   * asteroid.png: Asteroid sprite (~36x36 pixels, e.g., PNGAll).
   * logo.png, space\_bg.png, button.png: Placeholder images (e.g., Kenney.nl or OpenGameArt.org). Save all in the project directory.
2. **Compile the Code**: Save the C++ code as space\_shooter.cpp and run:

g++ space\_shooter.cpp -o space\_shooter -lraylib -lopengl32 -lgdi32 -lwinmm -lraymath

(Adjust for Linux with -lraylib -lGL -lm -lraymath.)

1. **Run the Game**: Launch space\_shooter to start your journey!

## **Controls**

* **Movement**: WASD or Arrow keys (LEFT/RIGHT to strafe, UP/DOWN to thrust).
* **Shooting**: SPACE (dual bullets with 0.18s cooldown).
* **Pause**: P key.
* **Menu Navigation**: UP/DOWN (W/S) to select, ENTER/SPACE to confirm.
* **Restart**: ENTER/SPACE in Game Over.
* **Exit Menu**: ESC in Game Over.

## **Gameplay**

* **Loading**: A 1.6-second progress bar welcomes you.
* **Menu**: Choose to start, visit Raylib, or quit.
* **Play**: Dodge and shoot asteroids, with difficulty increasing every 6 seconds (spawning speeds up).
* **Pause**: Toggle with P, return to menu with ENTER.
* **Game Over**: Restart with ENTER, return to menu with ESC.

## **Troubleshooting**

* **Images Not Loading**: Check console TraceLog for directory path. Ensure files are in the correct folder, named exactly, and are valid PNGs.
* **Compiler Errors**: Share the full output for further debugging.
* **Gameplay Issues**: Report odd asteroid movement or collisions—I’ll tweak speeds or hitboxes.

## **Future Enhancements**

Want to add sound (using PlaySound), more enemies, or power-ups? Just let me know, and I’ll sprinkle more magic into our adventure!
