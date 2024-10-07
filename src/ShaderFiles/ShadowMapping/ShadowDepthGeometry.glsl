#version 400

layout (triangles) in;
layout (triangle_strip, max_vertices = 18) out;

out vec4 FragPos;

uniform mat4 u_ShadowMatrices[6];
uniform bool u_IsOmnidir;

void main()
{
    if(u_IsOmnidir)
    {
        for(int f = 0; f < 6; ++f)
        {
            gl_Layer = f; //specification for which face we render
            for(int i = 0; i < 3; ++i)
            {
                //loop through vertices 0>>1>>2>>3
                FragPos = gl_in[i].gl_Position;
                gl_Position = u_ShadowMatrices[f] * FragPos;
                EmitVertex();
            }
            EndPrimitive();
        }
    }   
    else
    {
        //Since shader has geometry, this will affect directional light
        //depth output so emit vertices as usual
        for(int i = 0; i < gl_in.length(); i++)
        {
            FragPos = gl_in[i].gl_Position;
            gl_Position = FragPos;
            EmitVertex();
        }
        EndPrimitive();
    }

}