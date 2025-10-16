# ModernVoxelEngine

A next-generation **voxel-based rendering engine** built in modern C++, designed for **real-time global illumination**, **cross-API rendering**, and **scalable world simulation**.  
It supports both **Vulkan** and **DirectX 12**, aiming to push voxel-based worlds to the same visual fidelity as modern AAA engines.

---

## Overview

**ModernVoxelEngine** is a modular, high-performance voxel engine currently under active development.  
It serves as an experimental platform for exploring advanced rendering techniques — from voxel-based geometry streaming to path-traced global illumination — all powered by a unified multi-API abstraction layer.

The engine targets:
- Physically-based rendering (PBR)
- Massive voxel world streaming
- Real-time global illumination and reflections
- Cross-platform graphics backend (Vulkan + DirectX12)

---

## Current Progress

 Implemented:
- Core **graphics abstraction layer** (DX12 / Vulkan)  
- Scene rendering pipeline with forward shading  
- Camera system with free-flight control  
- GPU resource manager (VMA / Descriptor Pool / Handle System)  
- Chunk-based voxel world mesh generation  

In Progress:
- Data-driven resource system (shaders, materials, meshes)
- FrameGraph + RenderGraph system  
- PBR material shading  
- Cross-platform window abstraction (Win32 / GLFW)  

---

## Current Progress
10/10/2025 Finally rendered something up
![10/10/2025](ModernVoxelEngine/record/Sponza-10-10-2025.png)
![10/10/2025-unfix](ModernVoxelEngine/record/Sponza-10-10-2025-unfix.png)

## Quick Start (Visual Studio)

```bash
# Clone the repo
git clone https://github.com/shibatachun/FPSGame.git
cd FPSGame/ModernVoxelEngine

# Install dependencies (using vcpkg)
vcpkg integrate install
vcpkg install glfw3 volk glm glslang vulkan-headers vulkan-loader

# Open solution in Visual Studio
Open VoxelEngineVulkan.sln, enable vcpkg integration, set platform to x64

# Build
Select Debug or Release → Build Solution

# Run
Press F5 or run FPSGame.exe in the corresponding folder
```



