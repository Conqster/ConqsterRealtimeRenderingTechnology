# <p align = "center"> Deferred and Forward Rendering Comparison </p>

## Table of Content
- [Introdution/Scene Setup](#introdutionscene-setup)
    - [Shaders](#shaders)
- [G-Buffer](#g-buffer)
    - [Memory Size](#memory-size)
- [Lights](#lights)
    - [GPU Structure & Size](#gpu-structure--size)
- [Scene Models/Entities](#modelsentitiesobjects)
    - [Vertex Attributes](#vertex-attributes)
    - [Fragment Attributes](#fragment-attribute)
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
- Graphics API: OpenGL 
- Geometry Buffer (G-Buffer): 
    - 2 RGB textures (normal, position attribute).
    - 1 RGBA (base colour as RGB and specularity as A).
- Uniform buffer(UBO): 2 x UBO (camera & light attribute).
- Frame buffer (FBO): 2 X FBO (default, Multi-Render Target(MRT) for G-Buffer).
- Simplification
    - No shadow data calculation or storage.
    - Emphasis is placed on rendering efficiency over strict physical light accuracy.
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

### Scene Entities
- Models: Sponza
- Primitives:
    - Cube Primitive: cube, elongated cube for glass.
    - Plane/Quad Primitive: ground.
    - Sphere Primitive.

##  G-Buffer 
The G-Buffer uses a Multiple Render Target (MRT) setup to render the required properties of the scene in a single pass.

```
std::vector<FBO_TextureImageConfig> img_config = 
{
    //internalformat //datatype   //format
    {RGBA16F,       GL_FLOAT,     RGBA}, //<----------- Base colour(RGB) specular power (A)
    {RGB16F, GL_FLOAT, RGB}, //<----------------------- Normal attachment 
    {RGB16F, GL_FLOAT, RGB} //<------------------------ Position attachment
};
m_GBuffer.Generate(screen_width, screen_height, img_config);
```

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



## Lights

Types:
- Directional Light 
- Point Light (focus of the experiment)

### GPU Structure & Size

```
struct PointLight
{
    vec3 colour; //<--------- 12 bytes r 4 
    bool enable; //<--------- << 4 bytes 
    vec3 position; //<------- 12 bytes r 4
    float ambient; //<------- << 4 bytes
    vec3 attenuation; //<---- 12 bytes r 4
    float diffuse; //<------- << 4 bytes
    float specular; //<------ 4 bytes r 12
    float far; //<----------- << 4 bytes r 8
    vec2 padding; //<-------- << 8 bytes
};  //<--------------------------------------------- Total: 64 bytes

struct DirectionalLight
{
    vec3 colour; //<------------ 12 bytes r 4
    bool enable; //<------------ << 4 bytes
    vec3 direction; //<--------- 12 bytes r 4
    float ambinent; //<--------- << 4 bytes
    float diffuse; //<---------- 4 bytes r 12
    float specular; //<--------- << 4 bytes r 8
    vec2 padding; //<----------- << 8 bytes
}; //<----------------------------------------------- Total: 48 bytes

uniform int u_PtLightCount = 0;
const int MAX_POINT_LIGHTS = 1000; //<--------------- arbitrary value

layout (std140) uniform u_LightBuffer
{
    DirectionalLight dirLight;                  //aligned
    PointLight pointLights[MAX_POINT_LIGHTS];   //aligned
};
```
Key Details:
- `u_PtLightCount` governs the number of points in the scene. 
- The `vec3 colour` represent a consistent base colour for ambinet, diffuse, and specular light components, while individual floats scale thier intensities independently. 
- `Point Light - far` ominent for this experiment, but governs point light range perspective. 

Memory Usage:
- Directional Light: 48 bytes 
- Point Lights: *MAX_POINT_LIGHTS * 64 bytes = 1000 * 64 bytes* = 64,000 bytes
- *Total Size: 48 bytes + 64,000 bytes* = ~ 64 KB.

## Models/Entities/Objects

### Vertex Attributes

```
layout (location = 0) in vec4 pos;
layout (location = 1) in vec4 col;
layout (location = 2) in vec2 uv;
layout (location = 3) in vec3 nor;

layout (location = 4) in vec3 tangent;
layout (location = 5) in vec3 biTangent;

out VS_OUT
{
	vec3 fragPos; //<------------- Frag position
	vec2 UVs;     //<------------- Texture coord
	vec4 fragPosLightSpace; //<--- For Shadow mapping (not used in this experiment)
	mat3 TBN;     //<------------- Tangent-Bitangent-Normal matrix
	vec3 normal;  //<------------- Normal vector
}vs_out;


uniform mat4 u_Model; //<--------- model transformation matrix
```

### Fragment Attribute 
```
struct Material
{
	bool isTransparent;
	vec4 baseColour;
	
	sampler2D baseMap;
	sampler2D normalMap;
	bool useNormal;
	
	sampler2D parallaxMap;
	bool useParallax;
	float parallax;
	
	sampler2D specularMap;
	bool hasSpecularMap;
	
	int shinness;
};
```