#version 400

layout (triangles) in;
layout (triangle_strip, max_vertices = 18) out;

out vec4 FragPos;

uniform mat4 u_ShadowMatrices[6];


void main()
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