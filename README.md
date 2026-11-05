# Neon Night Street (Topic #21)

**Course:** Computer Graphics Fundamentals  
**Institution:** Astana IT University  
**Instructor:** Nazira Shaimerdenova  

---

## 👥 Team Members (Group SE-2423)
* **Vitaliy Golubenko** (@checkybox) — *Project setup, build configuration, FPS walking camera with street boundary limits, and vendored math library.*
* **Erasyl Meirambekov** — *Modular 3D street geometry, sidewalk curbs, and scene graph transformation pipeline.*
* **Ismail Zhanatay** — *Shader program wrapper, GLSL 330 pipeline, and vertex buffer abstractions.*
* **Nurbek Shagarbek** — *Window management, OpenGL 3.3 Core context setup, and event loop.*

---

## 🌆 Project Overview
An interactive 3D graphical application in modern OpenGL 3.3 Core profile depicting a rain-soaked urban street illuminated at night by multiple vivid neon storefronts, wet road reflections, and falling rain particles.

---

## 🛠️ Prerequisites & Build Setup
Target environment: C++11 and OpenGL 3.3+ Core Profile.
All math library headers (GLM) and OpenGL loader headers (GLAD) are self-contained within the repository.

### Linux (Ubuntu / WSL2)
Install prerequisites:
```bash
sudo apt-get update
sudo apt-get install -y build-essential libglfw3-dev libgl1-mesa-dev libx11-dev libxrandr-dev libxi-dev
```
Build and run:
```bash
make
./neon_night_street
```

### Cross-Platform (CMake)
```bash
mkdir build && cd build
cmake ..
cmake --build .
./neon_night_street
```

---

## 🎮 Controls
* **W / A / S / D:** Walk forward / left / backward / right (constrained to street walking plane).
* **Mouse Look:** Look around (pitch & yaw). Drag with Left Mouse Button or toggle lock.
* **Arrow Keys (← / → / ↑ / ↓):** Turn and pitch camera directly (universal fallback across all window managers).
* **C / Tab:** Toggle cursor capture mode (captured vs free mouse).
* **Esc:** Exit application.

---

## 🤖 AI Use Disclosure
AI tools (Google Gemini) are utilized during development for architectural brainstorming, code modularization, and drafting documentation. All generated code and shaders are reviewed, adapted, and maintained by the team members. Full details will be documented in the final report.
