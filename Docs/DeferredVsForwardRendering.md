# <p align = "center"> Deferred and Forward Rendering Comparison </p>

## Table of Content
- [Introdution/Scene Setup](#introdutionscene-setup)
    - [Shaders](#shaders)
- [G-Buffer](#g-buffer)
    - [Memory Size](#memory-size)
- Lights
- Rendering Paths
 - Forward Path
    - Opaque Pass
    - Tranparent Pass
 - Deferred Path
    - G-Buffer Pass
    - Lighting Pass
    - Transparent Pass
- Metrics & Performance
    - Forward Path
    - Deferred Path
- Extra Utilities
    - Scene Debugger

## Introdution/Scene Setup
- Screen Resolution: 2560 x 1440.
- Hardware API: OpenGL 
- Geometry Buffer (G-Buffer): 
    - 2 RGB texture (normal, position attribute).
    - RGBA (base colour as RGB and specularity as A).
- Uniform buffer(UBO): 2 x UBO (camera & light attribute).
- Frame buffer (FBO): 2 X FBO (default, Multi-Render Target(MRT) for G-Buffer).
- 
NOTE: NO SHADOW (SHADOW DATA CALCUALTION & STORAGE) OMIMENTED.
- Lights:
    - One Directional Light (On & Off).
    - Multiple Point Light count X<sup>(y*n-1)</sup>, where
        - (X): Progression base,
        - (y): Scaling factor,
        - (n): Step number.

- Scene src file: [ForwardVsDeferredScene.h](../src/Scene%20Graph/Scenes/ForwardVsDeferredRenderingScene.h) & [.cpp](../src/Scene%20Graph/Scenes/ForwardVsDeferredRenderingScene.cpp)

### Shaders

| Name           | Vertex               | Fragment               |
|----------------|----------------------|------------------------|
| Forward Shader | [StandardVertex.glsl](../Assets/Shaders/StandardVertex.glsl)| [StandardFrag.glsl](../Assets/Shaders/StandardFrag2.glsl)|
| G-Buffer Shader| [BasicVertexPos.vert](../Assets/Shaders/Experimental/BasicVertexPos.vert)| [GBufferFrag.glsl](../Assets/Shaders/GBufferFrag.glsl)|
| Deferred Shader| [DeferredShading.vert](../Assets/Shaders/Experimental/DeferredShading.vert)| [DeferredShading.frag](../Assets/Shaders/Experimental/DeferredShading.frag)|


##  G-Buffer 
The G-Buffer uses a Multiple Render Target (MRT) setup to render the required properties of the scene in a single pass.

### Memory Size

- Screen Resolution: 2560 x 1440
- Colour Attachments:
    - 16-bit per channel (*1 byte = 8 bits*):
    - 2 x RGB16F Textures: *2 * (16 + 16 + 16) = 2 * 48 = 96bits* = 12 bytes per pixel
    - 1 x RGBA16F Texture: *16 + 16 + 16 + 16 = 64 bits* = 8 bytes per pixel
    - Total Colour Attachment Size per pixel: *12 bytes/pixel + 8bytes/pixel* = 20 bytes per pixel
- Resolution and Texture Size:
    - Colour Attachment Total Size: *2560 * 1440 * 20 = 73,728,000 bytes* ~70.31 MB

- Render Buffer Storage for Depth: 
    - Depth Component: *GL_DEPTH_COMPONMENT16*
    - 16-bit per channel *(2 bytes per pixel):*
        - Depth Buffer Total Size: *2560 * 1440 * 2 = 7,372,800 bytes* ~7.04 MB

- Total Memory Usage:
    - Without Multisampling or Double Buffering (Ping-Pong Rendering):
        - Grand Total: *70.31 MB (Colour) + 7.04 MB (Depth))* ~77.35 MB