# Development Issues
 
# Table of Content
- Triangle winding
- Mesh Normals
- Cube Mesh texturing

## Triangle winding 
Had an issue during development has the indice i had set up for the OpenGL command "glDrawElements(GL_TRIANGLE_LOOP,......)" as one of the egde is doubled and another is open up with no line rendered.

## Mesh Normals 
At the start of development i had no normals, but as soon as a started lighting, i had to hack the normal "by using the vertex position as normal" in the shader program as i setup my thought i set up my vertex as 1 unit. but later realise it was 2 units. 
But eventually change it to 1 units and also add proper normals to my vertices.

## Cube Mesh Texturing