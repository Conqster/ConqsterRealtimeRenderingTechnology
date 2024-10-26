#version 400

out vec4 FragColour;


//////////////////////////////
//INs
//////////////////////////////
in VS_OUT
{
	vec2 texCoords;
	vec3 fragPos;
	vec3 normal; //transposed
	vec3 modelNor;
	vec4 colour;
	vec4 position;
	vec4 fragPosLightSpace;
}fs_in;

uniform vec3 u_GlowColour;
uniform float u_GlowIntensity;
uniform float u_GlowExp;

uniform sampler2D u_Texture;
uniform vec3 u_ModelColour;

uniform mat4 u_Model;
uniform vec3 u_ViewPos;

uniform float u_Time;
uniform float u_Speed;
uniform float u_IntensityOffset;

void main()
{
	float amb_strenght = 0.5f;
	vec3 base_col = texture(u_Texture, fs_in.texCoords).rgb * u_ModelColour * amb_strenght;
	
	//Extract the translation part of model matrix
	vec3 model_pos = vec3(u_Model[3][0], u_Model[3][1], u_Model[3][2]);
	vec3 view_dir = normalize(u_ViewPos - model_pos);
	
	float rim = 1.0f - clamp(dot(view_dir, fs_in.modelNor), 0.0f, 1.0f);
	float osci = sin(u_Time * u_Speed) * u_IntensityOffset;
	float intensity = u_GlowIntensity + osci;
	vec3 glow = u_GlowColour * intensity * pow(rim, u_GlowExp);
	
	vec4 emit_col = vec4(glow, 1.0f);
	
	FragColour = vec4(base_col, 1.0f) + emit_col;
	FragColour = emit_col;
	FragColour.a = rim;
}







