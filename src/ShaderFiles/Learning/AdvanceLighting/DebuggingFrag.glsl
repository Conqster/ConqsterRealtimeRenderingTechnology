#version 400

out vec4 FragColour;

in VS_OUT
{
	vec2 texCoords;
	vec3 fragPos;
	vec3 normal; //transposed
	vec3 modelNor;
	vec4 colour;
	vec4 position;
}fs_in;


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