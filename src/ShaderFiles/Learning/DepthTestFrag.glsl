#version 400

in vec3 v_FragPos;
in vec2 v_TexCoord;


out vec4 o_Colour;

uniform sampler2D u_Texture;
uniform float u_Intensity;
uniform bool u_DoDepthTest;

uniform float u_Near;
float far = 150.0f;

float LinearizeDepth(float depth)
{
	float z = depth * 2.0f - 1.0f;
	return (2.0f * u_Near * far) /(far + u_Near - z * (far - u_Near));
}

void main()
{
	//o_Colour = texture(u_Texture, v_TexCoord) + vec4(1.0f, 1.0f, 1.0f, 1.0f) * intensity;
	o_Colour = texture(u_Texture, v_TexCoord) * u_Intensity;
	
	
	if(!u_DoDepthTest)
		return;
	
	//playing with frag z-buffer/ depth
	
	float linear_depth = LinearizeDepth(gl_FragCoord.z) /far;
	
	//o_Colour = vec4(vec3(gl_FragCoord.z), 1.0f);
	o_Colour = vec4(vec3(linear_depth), 1.0f);
	
}