#version 400

layout (location = 0) in vec4 pos;
layout (location = 1) in vec4 col;
layout (location = 2) in vec2 uv;
layout (location = 3) in vec3 nor;

layout (std140)  uniform u_CameraMat
{
	mat4 projection;
	mat4 view;
};


//using modelFrag shader
out VS_OUT
{
	vec2 texCoords;
	vec3 fragPos;
	vec3 normal;
	vec3 modelNor;
	vec4 colour;
	vec4 position;
}vs_out;

uniform mat4 u_Model;

//instancing 
const int MAX_INSTANCE = 100;
uniform vec3 u_InstPosOffset[MAX_INSTANCE];


void main()
{
	//instance
	vec3 pos_offset = u_InstPosOffset[gl_InstanceID];
	vec3 instance_pos = pos.xyz + pos_offset;
	
	//instance model based on the instance offset position
	mat4 inst_model = u_Model;
	inst_model[3].xyz += u_Model[3].xyz + pos_offset;
	
	gl_Position = projection * view * inst_model * pos;

	//required properties for frag Shader
	vs_out.texCoords = uv;
	//vs_out.fragPos = vec3(u_Model * pos);
	vs_out.fragPos = vec3(inst_model * pos);

	mat3 nor_matrix = mat3(transpose(inverse(inst_model)));
	vs_out.normal = normalize(nor_matrix * nor);  
	
	vs_out.modelNor = nor;
	vs_out.colour = col;
	//vs_out.position = vec4(instance_pos, 1.0f);
	vs_out.position = pos;
}