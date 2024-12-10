#version 420 

//--------------attributes--------------/
layout (location = 0) in vec4 pos;
layout (location = 1) in vec4 col;
layout (location = 2) in vec2 uv;
layout (location = 3) in vec3 nor;

layout (location = 4) in vec3 tangent;
layout (location = 5) in vec3 biTangent;


out VS_OUT
{
	vec3 fragPos;
	vec2 UVs;
	vec4 fragPosLightSpace;
	mat3 TBN;
	vec3 normal;
	vec3 viewPos;
}vs_out;


out NVS_OUT
{
	vec4 posVS;  	//view space position
	vec2 UVs;    	//Texture coord
	vec3 tangentVS; //view space tangent
	vec3 normalVS;	//view space normal
	vec4 pos;		//clip space position
}nvs_out;
out vec2 vs_ScreenUV;
out vec4 vs_ClipSpace;
out mat4 vs_Proj;


//--------------uniform--------------/
layout (std140) uniform u_CameraMat
{
	vec3 viewPos;
	float far;
	mat4 proj;
	mat4 view;
	//mat3 padding;
	//vec4 padding;
};

uniform mat4 u_Model;
uniform mat4 u_DirLightSpaceMatrix;


void main()
{
	vec4 clip_pos = proj * view * u_Model * pos;
	
	vs_ScreenUV = viewPos.xy;
	
	gl_Position = clip_pos;
	vs_out.fragPos = vec3(u_Model * pos);
	vs_out.UVs = uv;
	vs_out.fragPosLightSpace = u_DirLightSpaceMatrix * u_Model * pos;
	
	vec3 t = normalize(vec3(u_Model * vec4(tangent, 0.0f)));
	vec3 n = normalize(vec3(u_Model * vec4(nor, 0.0f)));
	vec3 b = cross(n, t);
	
	vs_out.TBN = mat3(t, b, n);
	
	vs_out.viewPos = viewPos;
	
	vs_out.normal = mat3(transpose(inverse(u_Model))) * nor;
	nvs_out.posVS = clip_pos;
	nvs_out.UVs = uv;
	mat4 model_view = view * u_Model;
	nvs_out.tangentVS = mat3(model_view) * tangent;
	nvs_out.normalVS = mat3(model_view) * nor;
	//nvs_out.pos = (model_view * pos).xyz;
	nvs_out.pos = model_view * pos;
	
}