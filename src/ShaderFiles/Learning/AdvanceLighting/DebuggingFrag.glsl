#version 400

out vec4 FragColour;

uniform vec3 u_DebugColour; 
uniform bool u_Active;




void main()
{
	FragColour = vec4(vec3(0.0f), 1.0f);
	
	if(u_Active)
	{
		FragColour = vec4(u_DebugColour, 1.0f);
	}
}