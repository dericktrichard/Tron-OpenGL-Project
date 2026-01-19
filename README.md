# 3D Tron: High-Speed Battle Arena

A 3D implementation of the classic Tron Light Cycle game built using C++ and OpenGL (GLUT). This project features a chase-camera system, procedural light trails, and an AI opponent.

## Features
- **Dynamic 3D Camera:** Follows the player with a smooth bird's-eye perspective.
- **Smart AI:** An automated opponent that detects obstacles and tries to avoid self-collision.
- **Visuals:** Neon-style grid and light walls rendered using OpenGL primitives.
- **Collision Engine:** Handles wall, self, and opponent trail collisions.

## Prerequisites for WSL Users
To run this project on Windows Subsystem for Linux, you need:
1. **Windows 11** (for native GUI support) or an **X-Server** (like VcXsrv) for Windows 10.
2. **OpenGL Libraries:**
   ```bash
   sudo apt install build-essential freeglut3-dev libgl1-mesa-dev libglu1-mesa-dev# Tron 3D OpenGL Project
