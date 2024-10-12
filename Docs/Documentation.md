# Documentation

## Mesh Construction

### Square Mesh
Square Mesh are constructed with two CCW winded triangles with their normals facing towards forwards using the right hand where x is to right, y is to up and z is to forward.

### Cube Mesh 


### Sphere Mesh

|<a href="https://youtu.be/Uqg6LSkWq0U"><img src = "https://img.youtube.com/vi/Uqg6LSkWq0U/0.jpg"/>|
|:-|
|<p align = "center"> *A Youtube video of Sphere segment adjustment.* </p>|

## Lighting

|<a href="https://youtu.be/MPPAF9X-H8M"><img src = "https://img.youtube.com/vi/MPPAF9X-H8M/0.jpg"/>|
|:-|
|<p align = "center"> *A Youtube video of Sphere segment adjustment.* </p>|


## Light Source

### Point Light

|<a href="https://youtu.be/-_x3GDJ1Jv0"><img src = "https://img.youtube.com/vi/-_x3GDJ1Jv0/0.jpg"/>|
|:-|
|<p align = "center"> *A Youtube video of Sphere segment adjustment.* </p>|

### Spot Light

|<a href="https://youtu.be/g5ODVj54HSU"><img src = "https://img.youtube.com/vi/g5ODVj54HSU/0.jpg"/>|
|:-|
|<p align = "center"> *A Youtube video of Sphere segment adjustment.* </p>|

### Previous Scene Creation Method 

```
      //Game.h
  Scene* m_CurrentScene;
  ...
  ...
      //Game.cpp
//m_CurrentScene = new MainScene();
m_CurrentScene = new Light_ModelScene();
 ```