# OpenGL Rendering ~ Part of Game Tech Learning 

Welcome to my experimental learning project showcasing rendering techniques using OpenGL. Intially created for my 3D physics learning journey, I found Rendering quite fascinating. 

## Quick Start
Visual Studio 2022 was used for the development, with an x86/Debug and Windows build.

<ins>**1. Downloading the repository:**</ins>
Start by downloading a Zip from the "<> Code" option.

<ins>**2. Usage:**</ins>
Currently, a build script is not included, you can build directly in VS community (F5): 

- Build in Release/Debug Config, supporting platform x86.
- Multiply scene are abstracted into multple classes. To switch scenes, go to the .cpp class ([Game.cpp](3D-Rendering/src/Core/Game.cpp)) and change the current Scene pointer OnStart function. See current available scenes.

|```h
  //Game.h
  Scene* m_CurrentScene;
  ...
  ...
  ```|
|```cpp
  //Game.cpp
m_CurrentScene = new MainScene();
 ```|
|:-|
|<p align = "center> [Code Sample](3D-Rendering/src/Core/Game.cpp) </p>|

The Base Scene is a basic wrapper around OpenGL function and window creation, aiding my learning journey by allowing easy jumps into new rendering techniques without overwriting previous learnt techniques. 

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
- [Detailed Documentation](ReadMe_Docs/Documentation.md)
- [Learning Resources](#learning-resources)

## Goals
### Primary Goals
The primary aim of this project is to deepen my understanding of: 
-  3D Rendering techniques.
- Advance C++ programming techniques.
- Algorithms and data strutures relevant to game technologies.
- Memory management and optimization.
- Create a base Renderer for debugging my physics simulation in game development (through transfering 2D knowlegde to 3D).
  
### Secondary Goals
Includes:
- Program a solid basic 3D Renderer (current project) with Configuration type as dynamic linked libary (.dll).
- Return/continue with physics simulation (different project) after basis renderer (.dll) foundation is complete for linking.
- Advance Rendering simulation techniques with focus on lighting physics.
- Optimization techniques for rendering (Memory management). 
  

## Current Features
Implemented foundation game architecture and basic 3D renderer (model-view-projection and basic lighting) with OpenGL:
- Intergrated foundational game architecture components such as Game Loop, GameObject and Rendering pipeline.
- [Mesh construction](ReadMe_Docs/Documentation.md#mesh-construction)
  - [Square](ReadMe_Docs/Documentation.md#square-mesh)
  - [Cube](ReadMe_Docs/Documentation.md#cube-mesh)
  - [Sphere](ReadMe_Docs/Documentation.md#sphere-mesh)
- Camera/View (model-view-projection).
- [Lighting (Ambient + Diffuse)](ReadMe_Docs/Documentation.md#lighting).
- [Light Source](ReadMe_Docs/Documentation.md#light-source)
  - Directional
  - [Point](ReadMe_Docs/Documentation.md#point-light)
  - [Spot](ReadMe_Docs/Documentation.md#spot-light)

## Development Issues
Issues encounted during development:
- [Triangle winding](ReadMe_Docs/DevelopmentIssues.md#triangle-winding)
- [Mesh Normals](ReadMe_Docs/DevelopmentIssues.md#mesh-normals)
- [Cube Mesh Texturing](ReadMe_Docs/DevelopmentIssues.md#cube-mesh-texturing)
- 
For more detailed information, please refer to the [Development Issue](ReadMe_Docs/DevelopmentIssues.md)

## External Libraries
Current libaries in use: GLM, GLEW, GLFW, stb-image, Assimp.

  
## Documentation 
For more detailed information, please refer to the [Detailed Documentation](ReadMe_Docs/Documentation.md)

## Learning Resources
-[learnopengl.com](https://learnopengl.com/Getting-started)
-[The Cherno C++ videos](https://www.youtube.com/playlist?list=PLlrATfBNZ98dudnM48yfGUldqGD0S4FFb)
-OpenGl Programming Guide 9th Edition (Red Book)
-Mathematics for 3D Game Programming and Computer Graphics.

