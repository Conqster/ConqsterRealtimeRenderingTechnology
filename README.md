# 3D Computer Game Tech (Rendering & Physics Simulation)

Welcome to my 3D Computer Game Project, which serves as a experimenting/learning game technologies (physics simulation & rendering) ,applying memory management, optimization along with computer/game arcitecture.

|<img src = "ReadMe_Docs/Computer Graphics.png"/>|
|:-|
|<p align = "center"> *Current Milestone.* </p>|

## Goals

<p> The primary aim of this project is to deepen my understanding of: 

-  3D game development cycle
- Advance C++ programming techniques.
- Algorithms and data strutures relevant to game technologies.
- Memory management and optimization.
- 3D physics simulation in game development (through transfering 2D knowlegde to 3D).
- 3D Rendering techniques

</p>

### Sub-Goals 
<p> Secondary goals:

- Program a solid basic 3D Renderer (current project) with Configuration type as dynamic linked libary (.dll).
- Return/continue with physics simulation (different project) after basis renderer (.dll) foundation is complete for linking.
- Advance Rendering simulation techniques with focus on lighting physics.
- Optimization techniques for rendering (Memory management). 
  
</p>

## Current Features

<p> Implemented foundation game architecture and basic 3D renderer (model-view-projection and basic lighting) with OpenGL:

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
</p>

### External Libaries
<p>  Current libaries in use: GLM, GLEW, GLFW, stb-image.</p>




<div align = "center"> 
  
## Documentation 
</div>

<p> This section will dive into the discussion/description of what has been implement, below is a time-lapse of the current milestone to the project: </p>

<a href="README.md"><p>Click to open</p></a>

For more info, pls click[Test](README.md).

|<a href="https://www.youtube.com/watch?v=9skO5a-XUGg"><img src="https://img.youtube.com/vi/9skO5a-XUGg/0.jpg"/></a>|
|:-|
|<p align="center">*A Youtube video showing light source Testing.*</p>|




<video width="360" height="240" controls>
    <source src = "ReadMe_Docs/Spot & Point Light Experiment(Milestone showcase) NO SHADOWS(ambient, Diffuse &attenuation constants)2.mp4" type="video/mp4"/>
</video>

<a name="mesh-construction"></a>
<div align="center">
  
## Mesh Construction
</div>

### Square Mesh
