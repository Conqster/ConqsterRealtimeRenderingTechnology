//#version 400
#version 430 core

in vec2 v_UVCoord;
in vec4 v_Colour;

out vec4 FragColour;

uniform sampler2D u_ScreenTex;
uniform vec3 u_DebugColour;
uniform bool u_Debug;

layout(binding = 1) uniform sampler2DMS u_ScreenCaptureTex;
uniform int u_ViewWidth;
uniform int u_ViewHeight;
uniform int u_SampleCount;

void main()
{
	vec4 col = vec4(0.0f);
	
	
	//check for MSAA or MSAA2
	//for MSAA2: u_ViewWidth && u_ViewHeight will be set 
	
	if(u_ViewWidth == 0 || u_ViewHeight == 0)    //Do MSAA
	{
		col = texture(u_ScreenTex, v_UVCoord);
	}
	else                                   //Do MSAA2
	{
		ivec2 vpCoord  = ivec2(u_ViewWidth, u_ViewHeight);
		vpCoord.x = int(vpCoord.x * v_UVCoord.x);
		vpCoord.y = int(vpCoord.y * v_UVCoord.y);
		
		for(int i = 0; i < u_SampleCount; i++)
		{
			col += texelFetch(u_ScreenCaptureTex, vpCoord, i);
		}
		col /= u_SampleCount;
	}
	
	
	//add debug colour, if needed
	if(u_Debug)
	{
		float grayscale = 0.2126 * col.r + 0.7152 * col.g + 0.0722 * col.b;
		col = vec4(vec3(grayscale), 1.0f) * vec4(u_DebugColour, 1.0f);
	}
		
		
	//Final output
	FragColour = col;
}

