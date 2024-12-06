#version 400

layout (location = 0) in vec4 pos;

out vec3 v_TexCoords;

uniform mat4 u_Projection;
uniform mat4 u_View;

void main()
{
	v_TexCoords = pos.xyz;
	//gl_Position = (u_Projection * u_View * vec4(pos, 1.0f));
	//gl_Position = (u_Projection * u_View * vec4(pos, 1.0f)).xyww;
	gl_Position = (u_Projection * u_View * pos).xyww;
}
