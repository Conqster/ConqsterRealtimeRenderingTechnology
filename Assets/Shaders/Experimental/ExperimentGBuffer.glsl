#version 400

//--------------OUT--------------/
layout(location = 0) out vec4 o_BaseColour;
layout(location = 1) out vec4 o_Normal;
layout(location = 2) out vec4 o_Position;

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
void main()
{
	vec3 base_colour = u_Material.baseColour.rgb;
	base_colour *= texture(u_Material.baseMap, fs_in.uv).rgb;
	
	o_BaseColour = vec4(base_colour, 1.0f);
	o_Normal = vec4(normalize(fs_in.normal), 1.0f);
	o_Position = vec4(fs_in.fragPos, 1.0f);
}