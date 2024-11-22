#version 400

//--------------OUT--------------/
layout(location = 0) out vec4 o_BaseColour;
layout(location = 1) out vec4 o_TangentNormal;
layout(location = 2) out vec4 o_Position;
layout(location = 3) out vec4 o_Depth;

//--------------IN--------------/
in VS_OUT
{
	vec3 fragPos;
	vec3 normal;
	vec2 uv;
	mat3 TBN;
}fs_in;

//--------------------Material-----------------------------------------
struct Material
{
	bool isTransparent;
	vec4 baseColour;
	
	sampler2D baseMap;
	sampler2D normalMap;
	bool useNormal;
	
	sampler2D parallaxMap;
	bool useParallax;
	float parallax;
	
	int shinness;
};
//--------------uniform--------------/
uniform Material u_Material;
float far = 150.0f;
float near = 0.1f;

float LinearizeDepth(float depth)
{
	float z = depth * 2.0f - 1.0f;
	return (2.0f * near * far) /(far + near - z * (far - near));
}
void main()
{
	vec3 base_colour = u_Material.baseColour.rgb;
	base_colour *= texture(u_Material.baseMap, fs_in.uv).rgb;
	
	o_BaseColour = vec4(base_colour, 1.0f);
	float linear_depth = LinearizeDepth(gl_FragCoord.z) /far;
	o_Depth = vec4(vec3(linear_depth), 1.0f);
	
	vec3 N = normalize(fs_in.normal);
	if(u_Material.useNormal)
	{
		N = texture(u_Material.normalMap, fs_in.uv).rgb;
		N = N * 2.0f - 1.0f;
		N = normalize(fs_in.TBN * N);
	}
	o_TangentNormal = vec4(N, 1.0f);
	
	o_Position = vec4(fs_in.fragPos, 1.0f);
}