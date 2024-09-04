#version 400

layout (location = 0) in vec4 pos;
layout (location = 1) in vec4 col;
layout (location = 2) in vec2 uv;
layout (location = 3) in vec3 nor;

layout (std140)  uniform u_CameraMat
{
	mat4 projection;
	mat4 view;
	vec2 screen_res;
};

out VS_OUT
{
	vec4 colour;
	vec2 tex_coord;
}vs_out;

uniform mat4 u_Model;
uniform float u_AR;

uniform float u_Width;
uniform float u_Height;


//instancing 
uniform vec2 offsets[100];


void main()
{
	gl_Position = projection * view * u_Model * pos;
	gl_Position = vec4(pos.x, pos.y, 0.0f, 1.0f);
	
	
	gl_Position = pos * u_Model * vec4(((2.0f - vec2(u_Width, u_Height))/u_Height), 0.0f, 0.0f);
	
	vec2 offset = offsets[gl_InstanceID];
	gl_Position = projection * view * u_Model * vec4(pos.x + offset.x, pos.y + offset.y, pos.z, 1.0f); 
	
	//normalise position 
	vec4 fragPos = u_Model * pos;
	//gl_Position = vec4(((vec2(fragPos) * 2.0f - screen_res)/screen_res.y), 0.0f, 0.0f);
	
	vs_out.colour = col;
	vs_out.tex_coord = uv;
}