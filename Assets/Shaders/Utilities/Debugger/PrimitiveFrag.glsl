#version 400

out vec4 FragColour;

in VS_OUT
{
	vec3 colour; //vertex point colour
}fs_in;

uniform vec4 u_Colour; 
uniform bool u_DebugVertexCol;

void main()
{
	FragColour = u_Colour;
	
	if(u_DebugVertexCol)
		FragColour = vec4(fs_in.colour, 1.0f);
		
	FragColour = vec4(1.0f, 0.0f, 1.0f, 1.0f);
}