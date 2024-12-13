#version 400

layout(location = 0) in vec4 pos;
layout(location = 2) in vec2 uv;

out vec2 vs_TexCoords;
out vec3 vs_ViewPos;

//--------------uniform--------------/
layout (std140) uniform u_CameraMat
{
	vec3 viewPos;
	float far;
	mat4 proj;
	mat4 view;
};

void main()
{
	//gl_Position = pos;
	//scale up by 2.0f as the quad constructed is 1 unit, 
	//as in 0.5f to both side to make it ideal for a unit plane
	gl_Position = vec4(pos.x * 2.0f, pos.y * 2.0f, 0.0, 1.0); 
	//view, model, projection is not required as i am writing to screen quad
	vs_TexCoords = uv;
	
	vs_ViewPos = viewPos;
}