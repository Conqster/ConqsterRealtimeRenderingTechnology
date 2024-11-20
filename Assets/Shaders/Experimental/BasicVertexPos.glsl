#version 400

//--------------attributes--------------/
layout (location = 0) in vec4 pos;
layout (location = 1) in vec4 col;
layout (location = 2) in vec2 uv;
layout (location = 3) in vec3 nor;

layout (location = 4) in vec3 tangent;
layout (location = 5) in vec3 biTangent;

//--------------uniform--------------/
layout (std140) uniform u_CameraMat
{
	mat4 proj;
	mat4 view;
};


out VS_OUT
{
	vec3 fragPos;
	vec3 normal;
	vec2 uv;
	mat3 TBN;
}vs_out;

uniform mat4 u_Model;

void main()
{
	gl_Position = proj * view * u_Model * pos;
	vs_out.fragPos = vec3(u_Model * pos);
	vs_out.normal = mat3(transpose(inverse(u_Model))) * nor;
	
	vs_out.uv = uv;
	
	vec3 t = normalize(vec3(u_Model * vec4(tangent, 0.0f)));
	vec3 n = normalize(vec3(u_Model * vec4(nor, 0.0f)));
	vec3 b = cross(n, t);
	vs_out.TBN = mat3(t, b, n);
}