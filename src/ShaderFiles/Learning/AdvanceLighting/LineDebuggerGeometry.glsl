#version 400

layout (points, binding = 1) in;
layout (line_strip, max_vertex = 2) out;

out vec4 fp_Colour;

uniform vec3 u_DebugColour;
uniform bool u_Perform;

layout (std140)  uniform u_CameraMat
{
	mat4 projection;
	mat4 view;
};

void main()
{
	if(u_Perform)
	{
		fp_Colour = vec4(1.0f);
		mat4 proj_view = projection * view;
		//first pos
		gl_Position = proj_view * gl_in[0].gl_Position;
		EmitVertex();
		
		gl_Position = proj_view * gl_in[1].gl_Position;
		EmitVertex();
		
		EndPrimitve();
	}

	
}