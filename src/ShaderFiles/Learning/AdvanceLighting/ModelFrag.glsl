#version 400

out vec4 FragColour;

in VS_OUT
{
	vec2 texCoords;
}fs_in;

uniform sampler2D u_Texture;


void main()
{
	FragColour = vec4(0.0f, 0.0f, 1.0f, 1.0f) * texture(u_Texture, fs_in.texCoords);
	//FragColour = texture(u_Texture, fs_in.texCoords);
	
	
	//FragColour = vec4(1.0f, 0.0f, 1.0f, 1.0f);
}