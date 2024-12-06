#version 400

//--------------OUT--------------/
layout(location = 0) out vec3 o_BaseColourBuffer;  
layout(location = 1) out vec3 o_NormalBuffer;
layout(location = 2) out vec3 o_PositionBuffer;
layout(location = 3) out vec4 o_Specular_MatShinnessBuffer;


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
	
	sampler2D specularMap;
	bool hasSpecularMap;
	
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
	//////////
	//Base Colour 
	//////////
	vec3 base_colour = u_Material.baseColour.rgb;
	base_colour *= texture(u_Material.baseMap, fs_in.uv).rgb;
	o_BaseColourBuffer = base_colour;
	
	///////////
	//Normal Colour
	///////////
	vec3 N = normalize(fs_in.normal);
	if(u_Material.useNormal)
	{
		N = texture(u_Material.normalMap, fs_in.uv).rgb;
		N = N * 2.0f - 1.0f;
		N = normalize(fs_in.TBN * N);
	}
	o_NormalBuffer = N;
	

	////////////
	//Position Buffer
	////////////
	o_PositionBuffer = fs_in.fragPos;
	
	
	
	////////////
	//Specular & Specular power Buffer
	////////////
	vec3 specular = vec3(0.0f);
	if(u_Material.hasSpecularMap)
		specular = texture(u_Material.specularMap, fs_in.uv).rgb;
		
	o_Specular_MatShinnessBuffer = vec4(specular, u_Material.shinness);
}