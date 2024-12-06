#version 400

layout (location = 0) in vec4 pos;

out vec3 v_TexCoords;

layout (std140)  uniform u_CameraMat
{
	mat4 projection;
	mat4 view;
};

void main()
{
	v_TexCoords = pos.xyz;
	mat4 sky_view = mat4(mat3(view));//remove position column 3
	gl_Position = (projection * sky_view * pos).xyww;
}
