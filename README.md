# Conqster Realtime Rendering Technology (CRRT)

CRRT is personal learning project focused on real-time 3D rendering using OpenGL. Designed to explore, implement various rendering techniques and features. 

|<a href="https://youtu.be/BJatTR7e0eU"><img src = "Docs/Attempted Geometry Algebra - Topdown view.png"/>|
|:-|
|<p align = "center"> *A Youtube video of current Milestone with light rendering simulation.* </p>|

For more development [demos.](https://youtu.be/BJatTR7e0eU)

## Goals
The primary aim of this project is to deepen my understanding of: 
-  3D Rendering techniques.
- Advance my C++ knowledge.
- Algorithms and data strutures relevant to game technologies (rendering).
- Memory management and optimization.

## Current Features
### Rendering Techniques
- Deferred Rendering --> [Compared with Forward Rendering](Docs/DeferredVsForwardRendering.md)
- Environment Mapping (Skybox)
- Scene Rendering Entities Sorting (Spatial Ordering for Transparency)
- Debug Gizmos Primitive
- MRT (Multi-Render Target Framebuffer, for Geometric Buffer (GBuffer), multi render output etc)
- Light Source (Directional, Point & Spot)
- Omnidirectional Shadow mapping for Point Lights
- Texture Mapping (Diffuse map, Normal map, Parallax map)
- Directional Light Shadow mapping
- Multisample Anti-alising (MSAA)
- Object Instancing
- Screen Space Post Processing (Grayscale, Blurring, Edge detection)
- Scene property debugging (Model-World-Space Normals, Model Space Normal debugging)
- Mesh Primitive Construction
- Naive Gamma Correction

### Utilities
- Multiscene management
- Scene serialization and deserialization (for easy edit/modification).
- AABB Construction 
- Dyanmic AABB - Parent-children generation (~600ums for 500 hierarchy level).
- Frustum Culling (Dyanmic AABB intersection against Geometry planes).
- Model Loading via Assimp
- Image Loading via stb_image
- Editing UI via ImGui


## Quick Tech Samples
#### Omnidirectional Shadow (Point Lights)
|<img src = "Docs/Dynamic_omnidirectional_shadow.gif"/>|
|:-|
|<p align = "center"> *Omnidirectional Shadow, Point Light Shadows.* </p>|

#### Light highlight on Normal mapped models
|<img src = "Docs/Normal mapping with light higlight.png"/>|
|:-|
|<p align = "center"> *Light highlight on Normal mapped model.* </p>|

#### Blending (Transparency)
|<img src = "Docs/Transparency Ordering.gif"/>|
|:-|
|<p align = "center"> *Scene Entities Order by view distance (Transparency).* </p>|


#### Directional Shadow (Directional Light)
|<img src = "Docs/Dynamic_directional_shadow.gif"/>|
|:-|
|<p align = "center"> *Directional Shadow, Directional Light Shadows.* </p>|

#### Scene Renderer (Parent-children relation)
|<img src = "Docs/Scene Rendering Entities with Parent-Child Relationship.gif"/>|
|:-|
|<p align = "center"> *Model's rendered with material data instance, Material Based Modeling & Parent - children relation.* </p>|

#### Bounding Volume - AABB (Parent-children relation, Rendering optimisation)
|<img src = "Docs/AABB_test.gif"/>|
|:-|
|<p align = "center"> *Only root (parent is tracked) Bounding Volume to determine is any child is visible during sorting. Construction of individual parent (root node) AABB takes ~(0.0001ms - 0.0006ms).* </p>|

#### Dyanmic Multi-Render Target (MRT)
|<img src = "Docs/Experiment_wc_MRT4GBuffer.png"/>|
|:-|
|<p align = "center"> *MRT, Experimenting with MRT for GBuffer - Deferred Rendering.* </p>|

#### Skybox, Framebuffer
|<img src = "Docs/Skybox, Framebuffer, Depthtest.png"/>|
|:-|
|<p align = "center"> *Skybox, Framebuffer (renderpass), Depth test.* </p>|

#### MSAA (Post Process)
|<img src = "Docs/MSAA 1x-8x.gif"/>|
|:-|
|<p align = "center"> *Multisample Anti-alising 1x-8x, Post Processing.* </p>|

#### Scene Model Material  (Material Based Modeling)
|<img src = "Docs/render_scene_sample_wc_mat.png"/>|
|:-|
|<p align = "center"> *Model's rendered with material data instance, Material Based Modeling.* </p>|

### Debugging 
#### Scene Objects Debugging
|<img src = "Docs/debugging_scene.gif"/>|
|:-|
|<p align = "center"> *Debugging, Normals (Model Space, World Space).* </p>|

#### Geometry Planes
|<img src = "Docs/Planes (Frustum Plane).gif"/>|
|:-|
|<p align = "center"> *Experimenting with Planes for frustrum culling.* </p>|


## Documentation 
- [Quick Start.](Docs/QuickStart.md)
<!-- [Development Issues.](Docs/DevelopmentIssues.md).-->

<!--For more detailed information, please refer to the [Detailed Documentation.](Docs/Documentation.md)-->

## Dependencies
Current libaries in use: GLM, GLEW, GLFW, stb-image, Assimp, YAML-CPP.

## Learning Resources
- [learnopengl.com.](https://learnopengl.com/Getting-started)
- [The Cherno C++ videos.](https://www.youtube.com/playlist?list=PLlrATfBNZ98dudnM48yfGUldqGD0S4FFb)
- OpenGl Programming Guide 9th Edition (Red Book).
- Mathematics for 3D Game Programming and Computer Graphics - Eric Lengyel.
- Foundations of Game Engine Development (Volume 1 - Mathematics) Eric Lengyel.
- Foundations of Game Engine Development (Volume 2 - Rendering) Eric Lengyel.
- [Forward and Deferred Rendering - Cambridge Computer Science Talks.](https://youtu.be/n5OiqJP2f7w?si=QQGg5Mj2bPoBzMiu)
- [Forward vs Deferred vs Forward+ Rendering with DirectX 11.](https://www.3dgep.com/forward-plus/)
- [Physicall Based Shading in Theory & Practice.](https://blog.selfshadow.com/publications/s2013-shading-course/#course_content)
- Core Techniques and Algorithms in Game Programming.

## Models & Textures Acknowledgements 
- [Sci-fi-Electrical-Charger](https://sketchfab.com/3d-models/sci-fi-electrical-charger-free-download-f09b75dca41b40f6b1c3c64eca138094): by [JonParrish](https://sketchfab.com/JonParrish)
- [Patterned-Cobblestone-Texture](https://polyhaven.com/a/patterned_cobblestone_02): from [Poly-Haven](https://polyhaven.com/)
- [Patterned-Brick-Floor-Texture](https://polyhaven.com/a/patterned_brick_floor_02): from [Poly-Haven](https://polyhaven.com/)
- [Glass-background-Frosted-Pattern](https://www.freepik.com/free-photo/glass-background-with-frosted-pattern_18092852.htm#fromView=keyword&page=1&position=0&uuid=b22ead14-eeca-4b1a-af1c-f9b5c390cf0f): from [FreePik](https://www.freepik.com/)