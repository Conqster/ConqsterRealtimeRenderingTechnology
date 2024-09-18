#version 400

in vec2 v_UVCoord;
in vec4 v_Colour;

out vec4 FragColour;

uniform sampler2D u_ScreenTex;
uniform vec3 u_DebugColour;
uniform bool u_Debug;

void main()
{
	FragColour = vec4(0.0, 1.0, 0.0, 1.0);
	
	vec3 col = texture(u_ScreenTex, v_UVCoord).rgb;
	float grayscale = 0.2126 * col.r + 0.7152 * col.g + 0.0722 * col.b;
	FragColour = vec4(vec3(grayscale), 1.0);
	
	
	//FragColour = vec4(col, 1.0f);
	FragColour = texture(u_ScreenTex, v_UVCoord);
	
	
	if(u_Debug)
	{
		FragColour = vec4(vec3(grayscale), 1.0) * vec4(u_DebugColour, 1.0f);
		//FragColour = vec4(u_DebugColour, 1.0f);
	}
}

