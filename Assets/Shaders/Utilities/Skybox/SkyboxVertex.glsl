#version 400

layout (location = 0) in vec4 pos;

out vec3 v_TexCoords;

layout (std140)  uniform u_CameraMat
{
	vec3 viewPos;
	float far;
	mat4 proj;
	mat4 view;
};

void main()
{
	v_TexCoords = pos.xyz;
	mat4 sky_view = mat4(mat3(view));//remove position column 3
	gl_Position = (proj * sky_view * pos).xyww;
}
