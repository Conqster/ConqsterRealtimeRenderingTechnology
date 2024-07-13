# 3D Computer Game Tech (Rendering & Physics Simulation)

Welcome to my 3D Computer Game Project, which serves as a experimenting/learning game technologies (physics simulation & rendering) ,applying memory management, optimization along with computer/game arcitecture.

|<a href="https://www.youtube.com/watch?v=9skO5a-XUGg"><img src = "ReadMe_Docs/Computer Graphics.png"/>|
|:-|
|<p align = "center"> *A Youtube video of current Milestone with light rendering simulation.* </p>|

## Table of Contents
- [Goals](#goals)
  - [Primary Goals](#primary-goals)
  - [Secondary Goals](#secondary-goals)
- [Current Features](#current-features)
- [Development Issues](#development-issues)
- [External Libraries](#external-libraries)
- [Detailed Documentation](ReadMe_Doc/Documentation.md)

## Goals
### Primary Goals
The primary aim of this project is to deepen my understanding of: 
-  3D game development cycle
- Advance C++ programming techniques.
- Algorithms and data strutures relevant to game technologies.
- Memory management and optimization.
- 3D physics simulation in game development (through transfering 2D knowlegde to 3D).
- 3D Rendering techniques
  
### Secondary Goals
Includes:
- Program a solid basic 3D Renderer (current project) with Configuration type as dynamic linked libary (.dll).
- Return/continue with physics simulation (different project) after basis renderer (.dll) foundation is complete for linking.
- Advance Rendering simulation techniques with focus on lighting physics.
- Optimization techniques for rendering (Memory management). 
  

## Current Features
Implemented foundation game architecture and basic 3D renderer (model-view-projection and basic lighting) with OpenGL:
- Intergrated foundational game architecture components such as Game Loop, GameObject and Rendering pipeline.
- [Mesh construction](#mesh-construction)
  - [Square](#square-mesh)
  - Cube.
  - Sphere.
- Camera/View (model-view-projection).
- Lighting (Ambient + Diffuse).
- Light Source (Directional, Point, Spot).
- UI (for dev debugging/scenario setup)
  - Program/Objects stats.
  - Gameobject scrolling, properites modification, deletion.
  - Light objects scrolling, properties modification, deletion.
  - GLEW controls.

## Development Issues
Issues encounted during development:
- Triangle winding.
- Mesh Normals.
- Cube Mesh texturing.
- 
For more detailed information, please refer to the [Development Issue](ReadMe_Doc/DevelopmentIssue.md)

## External Libraries
Current libaries in use: GLM, GLEW, GLFW, stb-image.

  
## Documentation 
For more detailed information, please refer to the [Detailed Documentation](ReadMe_Doc/Documentation.md)
