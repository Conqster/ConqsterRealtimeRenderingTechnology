#version 400

layout (location = 0) in vec4 pos;

uniform mat4 u_LightSpaceMat;
uniform mat4 u_Model;
uniform bool u_IsOmnidir;

void main()
{
    if(u_IsOmnidir)
       gl_Position = u_Model * pos;
    else
        gl_Position = u_LightSpaceMat * u_Model * pos;
        
}