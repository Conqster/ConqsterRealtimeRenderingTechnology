#version 400

layout (location = 0) in vec4 pos;
layout (location = 1) in vec4 col;
layout (location = 2) in vec2 uv;
layout (location = 3) in vec3 nor;

layout (location = 4) in vec3 tangent;
layout (location = 5) in vec3 biTangent;


out VS_OUT
{
	vec2 texCoords;
	vec3 fragPos;
	vec3 normal;
	vec3 modelNor;
	vec4 colour;
	vec4 position;
	vec4 fragPosLightSpace;
	mat3 TBN;
}vs_out;

layout (std140) uniform u_CameraMat
{
	mat4 projection;
	mat4 view;
};

uniform mat4 u_Model;
uniform mat4 u_LightSpaceMatrix;


void main()
{
	gl_Position = projection * view * u_Model * pos;
	
	vs_out.texCoords = uv;
	vs_out.fragPos = vec3(u_Model * pos);
	vs_out.normal = mat3(transpose(inverse(u_Model))) * nor;
	vs_out.modelNor = nor;
	vs_out.colour = col;
	vs_out.position = pos;
    
	
	vs_out.fragPosLightSpace = u_LightSpaceMatrix * u_Model * pos;
	
	//this could be done in the CPU 
	vec3 t = normalize(vec3(u_Model * vec4(tangent, 0.0f)));
	//vec3 b = normalize(vec3(u_Model * vec4(biTangent, 0.0f)));
	vec3 n = normalize(vec3(u_Model * vec4(nor, 0.0f)));
	vec3 b = cross(n, t);
	vs_out.TBN = mat3(t, b, n);
}