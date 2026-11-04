# Neon Night Street (Topic #21)

**Course:** Computer Graphics Fundamentals  
**Institution:** Astana IT University  
**Instructor:** Nazira Shaimerdenova  

---

## 👥 Team Members (Group SE-2423)
* **Vitaliy Golubenko** (@checkybox) — *Project setup and repository initialization; roles to be assigned as modules develop.*
* **Erasyl Meirambekov** — *Role to be assigned.*
* **Ismail Zhanatay** — *Shader program wrapper, GLSL 330 pipeline, and vertex buffer abstractions.*
* **Nurbek Shagarbek** — *Window management, OpenGL 3.3 Core context setup, and event loop.*

---

## 🌆 Project Overview
An interactive 3D graphical application in modern OpenGL 3.3 Core profile depicting a rain-soaked urban street illuminated at night by multiple vivid neon storefronts, wet road reflections, and falling rain particles.

---

## 🛠️ Prerequisites & Build Setup
Target environment: C++11 and OpenGL 3.3+ Core Profile.

### Linux (Ubuntu / WSL2)
Install prerequisites:
```bash
sudo apt-get update
sudo apt-get install -y build-essential libglfw3-dev libgl1-mesa-dev libglm-dev libx11-dev libxrandr-dev libxi-dev
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
* **Esc:** Exit application.
*(Additional navigation and interaction controls will be documented as modules are integrated).*

---

## 🤖 AI Use Disclosure
AI tools (Google Gemini) are utilized during development for architectural brainstorming, code modularization, and drafting documentation. All generated code and shaders are reviewed, adapted, and maintained by the team members. Full details will be documented in the final report.
