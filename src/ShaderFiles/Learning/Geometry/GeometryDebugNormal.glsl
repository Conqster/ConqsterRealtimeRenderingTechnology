#version 400

layout (triangles) in;
layout (line_strip, max_vertices = 6) out;


in VS_OUT
{
	vec3 normal;
	vec4 colour;
}gs_in[];


out vec4 f_Colour;

//Uniform buffer to use projection
layout (std140)  uniform u_CameraMat
{
	mat4 projection;
	mat4 view;
};

uniform float u_NorDebugLength;
uniform vec3 u_DebugColour;
uniform bool u_UseDebugColour;


void GenerateVertexLine(int idx)
{
	vec4 v_pos = gl_in[idx].gl_Position;
	
	if(u_UseDebugColour)
		f_Colour = vec4(u_DebugColour, 1.0f);
	else
	{
		f_Colour = gs_in[idx].colour;
		f_Colour.a = 1.0f;
	}
	
	gl_Position = projection * v_pos;
	EmitVertex();
	
	gl_Position = projection * (v_pos + vec4(gs_in[idx].normal, 0.0f) * u_NorDebugLength);
	EmitVertex();
	
	EndPrimitive();
}

void main()
{
	GenerateVertexLine(0);
	GenerateVertexLine(1);
	GenerateVertexLine(2);
}
