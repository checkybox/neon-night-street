# Neon Night Street (Topic #21)

Course: Computer Graphics Fundamentals  
Institution: Astana IT University  
Instructor: Nazira Shaimerdenova  

### Team Members (Group SE-2423)
* Vitaliy Golubenko (@checkybox) — Project architecture, build systems (CMake/Makefile), first-person walking camera with physical collision boundaries, 3D spatial audio engine (miniaudio), dynamic rain particle simulation, performance optimization, and final integration.
* Erasyl Meirambekov — Modular 3D scene architecture, building facades, alleyways, entrances, hotel structure, street props, and hierarchical scene graph pipeline.
* Ismail Zhanatay — Shader program architecture, texture mapping with mipmapping, multi-pass shader pipeline (scene, emissive, skyscraper, decal), and procedural wet road specular puddle reflections.
* Nurbek Shagarbek — Core windowing loop, GLFW context configuration, Blinn-Phong lighting model, 32 point lights with distance attenuation, directional moon lighting, and interactive flashlight spotlight.

### Project Overview
An interactive real-time 3D graphical application built with modern OpenGL 3.3 Core profile depicting a rain-soaked cyberpunk avenue illuminated at night by glowing neon storefronts, wet road reflections, volumetric fog, falling rain particles, and spatial ambient audio.

### Technical Architecture
* Multi-Shader Rendering Pipeline: Specialized shaders for standard lit scene geometry, high-intensity emissive neon signs, skybox skyscraper facades, transparent decal projections, and rain particle streaks.
* Tone Mapping: ACES filmic tone mapping curve applied across all visual passes for rich cinematic contrast without highlight clipping.
* Advanced Lighting: Blinn-Phong lighting supporting directional moonlight, player flashlight spotlight with smooth cutoff attenuation, and 32 colored point lights.
* Procedural Wet Road: Multi-octave Fractal Brownian Motion (FBM) in world space generating organic wet puddle boundaries, localized diffuse darkening, and dynamic specular sharpening.
* Procedural Rain Simulation: Dynamic line-streak particle system centered on the player with vertical gravity and atmospheric opacity fading.
* 3D Spatial Audio: Pure C procedural audio engine powered by miniaudio with distance-attenuated synthesizer soundscapes, spatialized transformer buzz, and footstep audio.
* Front-to-Back Rendering: Depth sorting on opaque scene nodes maximizing hardware early-z rejection for consistent real-time performance across high-resolution displays and integrated GPUs.

### Prerequisites and Build Setup
Target environment: C++11 and OpenGL 3.3 Core Profile.  
All math libraries (GLM), OpenGL loaders (GLAD), and audio headers (miniaudio) are self-contained within the repository. Windows standalone builds include bundled GLFW static library binaries in `lib/`.

#### Linux (Ubuntu / Debian / WSL2)
Install dependencies:
```bash
sudo apt-get update
sudo apt-get install -y build-essential cmake libglfw3-dev libgl1-mesa-dev libx11-dev libxrandr-dev libxi-dev
```

#### Linux (Arch Linux)
Install dependencies:
```bash
sudo pacman -S --needed base-devel cmake glfw mesa libx11 libxrandr libxi
```

#### Building on Linux
Using Make:
```bash
make
./neon_night_street
```
Using CMake:
```bash
mkdir build && cd build
cmake ..
cmake --build .
./neon_night_street
```

#### Building on Windows (Visual Studio / CMake)
Using PowerShell or Command Prompt:
```shell
mkdir build && cd build
cmake ..
cmake --build . --config Release
./Release/neon_night_street
```

### Controls
* W / A / S / D: Walk forward, left, backward, right with physical street boundaries.
* Mouse Look: Look around (pitch and yaw). Drag with Left Mouse Button or toggle mouse capture.
* Arrow Keys: Turn and pitch camera directly (universal keyboard fallback).
* C / Tab: Toggle mouse cursor capture mode (captured vs free pointer).
* F: Toggle player flashlight spotlight.
* [ / ]: Decrease or increase player flashlight brightness.
* G: Toggle atmospheric night fog.
* H: Trigger immediate procedural neon electrical flicker surge and transformer crackle.
* B: Toggle full city electrical blackout and staggered grid restoration with industrial breaker audio.
* Space: Toggle cinematic movie cutscene featuring wet asphalt tracking, sequential light ignition, dynamic camera angles, and cyberpunk synth music.
* R: Cycle rain intensity (Normal -> Intense -> Storm -> Off) with synchronized audio volume.
* K: Toggle backface culling (GL_CULL_FACE) for performance testing.
* Esc: Exit application or dismiss active cinematic cutscene.

### AI Use Disclosure
AI tools (Google Gemini) were utilized during development for architectural brainstorming, performance investigation, code modularization, and documentation drafting. All generated code, math, and shaders were reviewed, adapted, verified, and defended by the team members.
