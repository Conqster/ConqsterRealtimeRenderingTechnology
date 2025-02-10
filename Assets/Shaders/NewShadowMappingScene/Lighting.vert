#version 400

layout (location = 0) in vec4 pos;
layout (location = 1) in vec4 col;
layout (location = 2) in vec2 uv;
layout (location = 3) in vec3 nor;

layout (location = 4) in vec3 tangent;
layout (location = 5) in vec3 biTangent;

layout (std140) uniform u_CameraMat
{
	vec3 viewPos;
	float far;
	mat4 proj;
	mat4 view;
};


out VS_OUT
{
	vec3 fragPos;
	vec3 viewPos;
	vec3 normal;
	vec4 fragPosLightSpace;
}vs_out;

uniform mat4 u_Model;
uniform mat4 u_LightSpaceMat;

void main()
{
	vec4 v_model_pos = u_Model * pos;
	
	gl_Position = proj * view * v_model_pos;
	
	vs_out.fragPos = v_model_pos.xyz;
	vs_out.viewPos = viewPos;
	vs_out.normal = mat3(transpose(inverse(u_Model))) * normalize(nor);
	
	vs_out.fragPosLightSpace = u_LightSpaceMat * u_Model * pos;
}