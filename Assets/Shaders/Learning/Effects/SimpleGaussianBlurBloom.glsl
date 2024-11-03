#version 330 core
out vec4 FragColour;

in vec2 v_UVCoord;

uniform sampler2D u_BloomTex;

uniform bool horizontal;
uniform float rate;

//temporally using gaussian blur's 
//from [learn opengl] https://learnopengl.com/Advanced-Lighting/Bloom
//000000000000000000000
//00000000#####00000000
//00000###########00000
//000###############000
//00000###########00000
//00000000#####00000000
//000000000000000000000
uniform float weight[5] = float[](0.227027, 0.1945946, 0.1216216, 0.054054, 0.016216);

void main()
{
	vec2 tex_offset = 1.0f /textureSize(u_BloomTex, 0); 
	vec3 result = texture(u_BloomTex, v_UVCoord).rgb * weight[0];
	
	if(horizontal)
	{
		for(int i = 1; i < 5; ++i)
		{
			result += texture(u_BloomTex, v_UVCoord + vec2(tex_offset.x * i, 0.0f)).rgb * weight[i] * rate;
			result += texture(u_BloomTex, v_UVCoord - vec2(tex_offset.x * i, 0.0f)).rgb * weight[i] * rate;
		}
	}
	else
	{
		for(int i = 1; i < 5; ++i)
		{
			result += texture(u_BloomTex, v_UVCoord + vec2(0.0f, tex_offset.y * i)).rgb * weight[i] * rate;
			result += texture(u_BloomTex, v_UVCoord - vec2(0.0f, tex_offset.y * i)).rgb * weight[i] * rate;
		}
	}
	
	FragColour = vec4(result, 1.0f);
}