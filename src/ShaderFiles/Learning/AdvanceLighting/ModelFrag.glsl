#version 400

out vec4 FragColour;

in VS_OUT
{
	vec2 texCoords;
	vec3 fragPos;
	vec3 normal;
	vec4 colour;
	vec4 position;
}fs_in;

//////////////////////////////
//LIGHTS
//////////////////////////////
struct Light
{
	vec3 position;
	vec3 colour;
};


///////////////////////////////
//UNIFORMS
///////////////////////////////
uniform sampler2D u_Texture;

uniform Light u_Light;
uniform vec3 u_ViewPos;  //in-use camera pos
uniform int u_Shininess;

//debuggers
uniform bool u_DebugLightLocation; 
uniform bool u_DebugScene;
uniform bool u_DisableTex;
uniform int u_DebugWcType;

#define DEBUG_WC_MODEL_SPACE 0
#define DEBUG_WC_MODEL_NOR 1
#define DEBUG_WC_MODEL_ABS_NOR 2
#define DEBUG_WC_MODEL_COLOUR 3
#define DEBUG_DEFAULT_COLOUR 4



vec3 CalLight()
{
	//get frag dir to light 
	//get frag dir to veiw
	//halway dir (mid vector btw light dir & view dir 
	
	vec3 light_dir = normalize(u_Light.position - fs_in.fragPos);
	vec3 view_dir = normalize(u_ViewPos - fs_in.fragPos);
	//vec3 halway_dir = normalize(light_dir + view_dir);
	vec3 halway_dir = light_dir + view_dir;
	
	
	float spec = pow(max(dot(fs_in.normal, halway_dir), 0.0f), u_Shininess);
	vec3 specularCol = u_Light.colour * spec;
	return specularCol;
}


void main()
{
	//to make texture contribute, if one is present
	if(!u_DisableTex)
		FragColour = texture(u_Texture, fs_in.texCoords);// * vec4(vec3(0.1f), 0.0f); //to reduce the brightness of texture
	
	//FragColour = vec4(1.0f, 0.0f, 1.0f, 1.0f);
	FragColour += vec4(CalLight(), 1.0f);
	
	

	
	
	if(u_DebugLightLocation)
	{
		FragColour += fs_in.colour;
	}
	
	
	
	if(u_DebugScene)
	{
		if(u_DebugWcType == DEBUG_WC_MODEL_SPACE)
			FragColour = abs(fs_in.position);
		else if(u_DebugWcType == DEBUG_WC_MODEL_NOR)
			FragColour = vec4((fs_in.normal), 1.0f);
		else if(u_DebugWcType == DEBUG_WC_MODEL_ABS_NOR)
			FragColour = vec4(abs(fs_in.normal), 1.0f);
		else if(u_DebugWcType == DEBUG_WC_MODEL_COLOUR)
			FragColour = fs_in.colour;
		else
			FragColour = vec4(1.0f, 0.0f, 1.0f, 1.0f);//default to magenta
	}
}