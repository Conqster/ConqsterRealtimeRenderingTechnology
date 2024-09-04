# OpenGL Rendering ~ Part of Game Tech Learning 

Welcome to my ongoing experimental learning project showcasing rendering techniques using OpenGL. Initially created for my 3D physics learning journey, I found Rendering quite fascinating. 

## Quick Start
Visual Studio 2022 was used for the development, with an x86/Debug and Windows build.

### 1. Downloading the repository

Start by downloading a Zip from the "<> Code" option.

### 2. Building the Project

A build script is not included. You can build the project directly in Visual Studio Community Edition (Press F5): 

- Build Configuration: Choose either Release or Debug.
- Supported Platform: x86.
### 3. Using Scene Manager
The project uses a `SceneManager` to handle multiple scene, each abstracted into separate classes. 

#### Registering Scenes
Scenes are registed with the `SceneManager` using a template class and a uniques string key for later retrieval.
In `Game.cpp` (within the Init function):
```
//Initialise scene manager
m_SceneManger = new SceneManager();

//Register scenes with a unique key
m_SceneManger->RegisterNewScene<MainScene>("Main Scene");
m_SceneManager->RegisterNewScene<Texture_FrameBufferScene>("Haha Frame buffer");
```
#### Loading and Storing Scenes:
To load a registered scene, use `LoadScene(const std::string& name_key, Window* window)`. This method returns `nullptr` if the scene associated with `name_key` doesn't exist:
 ```
m_CurrentScene = m_SceneManager->LoadScene("Haha Frame buffer", m_Window);
```
#### Dynamically Unloading and Loading Scenes:
To unload the current scene and load another:
 ```
m_CurrentScene->OnDestroy();
m_CurrentScene = nullptr;
m_CurrentScene = m_SceneManager->LoadScene(m_SceneManager->ScenesByNamePtr()[1], m_Window);
 ```
You can also access scenes by index: `ScenesByNamePrt()` returns an array of scene names, which can be used to load scenes dynamically by their index.
### 4. Scene
The Base `Scene` class is a wrapper around OpenGL functions and window creation, designed to facilitate learning by allowing you to experiment with new rendering techniques without overwriting previous leaned methods.

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
- Instancing
- Model loading via Assimp

## Development Issues
Issues encounted during development:
- [Triangle winding](ReadMe_Docs/DevelopmentIssues.md#triangle-winding)
- [Mesh Normals](ReadMe_Docs/DevelopmentIssues.md#mesh-normals)
- [Cube Mesh Texturing](ReadMe_Docs/DevelopmentIssues.md#cube-mesh-texturing)
- 
For more detailed information, please refer to the [Development Issue](ReadMe_Docs/DevelopmentIssues.md)

## Some Screenshots
### Skybox, Framebuffer

|<img src = "ReadMe_Docs/Skybox, Framebuffer, Depthtest.png"/>|
|:-|
|<p align = "center"> *Skybox, Framebuffer (renderpass), Depth test.* </p>|

## External Libraries
Current libaries in use: GLM, GLEW, GLFW, stb-image, Assimp.

  
## Documentation 
For more detailed information, please refer to the [Detailed Documentation](ReadMe_Docs/Documentation.md)

## Learning Resources
- [learnopengl.com](https://learnopengl.com/Getting-started)
- [The Cherno C++ videos](https://www.youtube.com/playlist?list=PLlrATfBNZ98dudnM48yfGUldqGD0S4FFb)
- OpenGl Programming Guide 9th Edition (Red Book)
- Mathematics for 3D Game Programming and Computer Graphics - Eric Lengyel.
- Foundations of Game Engine Development (Volume 2 - Rendering) Eric Lengyel
