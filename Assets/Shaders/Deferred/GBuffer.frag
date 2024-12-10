#version 400

//--------------OUT--------------/
layout(location = 0) out vec4 o_BaseSpecBuffer;  
layout(location = 1) out vec3 o_NormalBuffer;
layout(location = 2) out vec3 o_PositionBuffer;
layout(location = 3) out vec3 o_DepthBuffer;


//--------------IN--------------/
in VS_OUT
{
	vec3 fragPos;
	vec3 normal;
	vec2 uv;
	mat3 TBN;
	float camFar;
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

float LinearizeDepth(float depth, float near, float far)
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
	o_BaseSpecBuffer.rgb = base_colour;
	
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
	float specular_value = 0.0f;
	if(u_Material.hasSpecularMap)
		specular_value = texture(u_Material.specularMap, fs_in.uv).r;
		
		
	
	//Store specular map red & shinness / power in o_BaseSpecBuffer's alpha
	//for now add the specular value with material shiness /power;
	o_BaseSpecBuffer.a = specular_value + u_Material.shinness;
	
	float linear_depth = LinearizeDepth(gl_FragCoord.z, 0.1f,  fs_in.camFar) /fs_in.camFar;
	o_DepthBuffer = vec4(vec3(linear_depth), 1.0f).rgb;
}