#version 400

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
}vs_out;


//--------------uniform--------------/
layout (std140) uniform u_CameraMat
{
	mat4 proj;
	mat4 view;
};

uniform mat4 u_Model;
uniform mat4 u_DirLightSpaceMatrix;


void main()
{
	gl_Position = proj * view * u_Model * pos;
	
	vs_out.fragPos = vec3(u_Model * pos);
	vs_out.UVs = uv;
	vs_out.fragPosLightSpace = u_DirLightSpaceMatrix * u_Model * pos;
	
	
	
	vec3 t = normalize(vec3(u_Model * vec4(tangent, 0.0f)));
	vec3 n = normalize(vec3(u_Model * vec4(nor, 0.0f)));
	vec3 b = cross(n, t);
	vs_out.TBN = mat3(t, b, n);
	
	vs_out.normal = mat3(transpose(inverse(u_Model))) * nor;
	
}