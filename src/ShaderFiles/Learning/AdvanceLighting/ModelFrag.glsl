#version 400

out vec4 FragColour;

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

//////////////////////////////
//LIGHTS
//////////////////////////////
const int MAX_LIGHTS = 7;
struct Light
{
	bool is_enable;
	vec3 position;     
	vec3 colour;
	
	bool is_directional;
	vec3 direction;
	
	float ambinentIntensity;
	
	//float constantAttenuation;
	//float linearAttenuation;
	//float quadraticAttenuation;
	//TO-DO: Stored all attenuation contiguously in memory for easy access
	//		Probably change to a vec3 later
	vec3 attenuation;
};


///////////////////////////////
//UNIFORMS
///////////////////////////////
uniform sampler2D u_Texture;
uniform sampler2D u_ShadowMap;
uniform Light u_Lights[MAX_LIGHTS];
uniform int u_LightCount; 
uniform vec3 u_ViewPos;  //in-use camera pos
uniform int u_Shininess;
uniform bool u_Blinn_Phong;
uniform bool u_GammaCorrection;
uniform float u_Gamma;

//debuggers
uniform bool u_DebugScene;
uniform bool u_DisableTex;
uniform int u_DebugWcType;

#define DEBUG_WC_MODEL_SPACE 0
#define DEBUG_WC_NORMAL 1
#define DEBUG_WC_MODEL_NOR 2
#define DEBUG_WC_MODEL_ABS_NOR 3
#define DEBUG_WC_MODEL_COLOUR 4
#define DEBUG_DEFAULT_COLOUR 5


float ShadowCalculation(vec4 fragPosLightSpace)
{
	vec3 projCoords = fragPosLightSpace.xyz / fragPosLightSpace.w;
	
	projCoords = projCoords * 0.5f + 0.5f;
	float closestDepth = texture(u_ShadowMap, projCoords.xy).r;
	float currentDepth = projCoords.z;
	float bias = 0.005f;
	bias = max(0.05f * (1.0f - dot(normalize(fs_in.normal), u_Lights[5].direction)), 0.005f);
	bias = 0.0f;
	float shadow = currentDepth - bias > closestDepth ? 1.0f : 0.0f;
	
	return shadow;
}


//For Quick test 
vec3 CalculateDirLight(Light light)
{
	vec3 result_colour = vec3(0.0f);
	
	//if(light.is_directional)
	//{
		if(!light.is_enable)
			return vec3(0.0f);
		
		//////////////////////
		//Ambinent
		//////////////////////
		vec3 amb_colour = light.ambinentIntensity * light.colour;
		
		/////////////////////
		//Diffuse
		/////////////////////
		//vec3 light_dir = normalize(light.position - fs_in.fragPos);
		vec3 light_dir = normalize(light.direction);
		vec3 nor = normalize(fs_in.normal);
		//float diffuse = max(dot(light.direction, nor), 0.0f);
		float diff_factor = max(dot(nor, light_dir), 0.0f);
		
		if(diff_factor <= 0.0f)
			return amb_colour + (light.colour * diff_factor);
		
		///////////////
		//Spec
		///////////////
		float spec = 0.0f;
		vec3 view_dir = normalize(u_ViewPos - fs_in.fragPos);
		if(u_Blinn_Phong)
		{
			vec3 halway_dir = normalize(light_dir + view_dir);
			spec = pow(max(dot(nor, halway_dir), 0.0f), u_Shininess);
		}
		else
		{
			vec3 reflectDir = reflect(-light_dir, nor);
			spec = pow(max(dot(view_dir, reflectDir), 0.0f), u_Shininess);
		}
		
		float shadow = ShadowCalculation(fs_in.fragPosLightSpace);
		vec3 lighting = (amb_colour + (1.0f - shadow) * (diff_factor + spec)) * light.colour;
	
		return lighting;
		vec3 scatteredLight = light.colour * diff_factor;
		vec3 reflectedLight = light.colour * spec;
		return amb_colour + scatteredLight + reflectedLight;
	//}
	
	
	//Not a directional light;
	//return vec3(0.0f);
}


vec3 CalculatePointLight(vec3 object_ambient_colour)
{
	vec3 result_colour = vec3(0.0f);
	
	//return CalculateDirLight(u_Lights[5]) * object_ambient_colour;
	
	for(int i = 0; i < u_LightCount; i++)
	{
		if(i > MAX_LIGHTS)
			break;
			
		if(!u_Lights[i].is_enable)
			continue;
			
		if(u_Lights[i].is_directional)
		{
			result_colour += CalculateDirLight(u_Lights[i]);
			break;
		}
			
			
		///////////
		//Ambinent
		///////////
		vec3 amb_colour = u_Lights[i].ambinentIntensity * u_Lights[i].colour;
		
		///////////
		//Diffuse
		///////////
		vec3 light_dir = normalize(u_Lights[i].position - fs_in.fragPos);
		vec3 nor = normalize(fs_in.normal);
		float diffuse = max(dot(light_dir, nor), 0.0f);
		
		
		///////////////
		//Spec
		///////////////
		float spec = 0.0f;
		vec3 view_dir = normalize(u_ViewPos - fs_in.fragPos);
		if(u_Blinn_Phong)
		{
			vec3 halway_dir = normalize(light_dir + view_dir);
			spec = pow(max(dot(nor, halway_dir), 0.0f), u_Shininess);
		}
		else
		{
			vec3 reflectDir = reflect(-light_dir, nor);
			spec = pow(max(dot(view_dir, reflectDir), 0.0f), u_Shininess);
		}
		
		
		//Attenuation
		float distance = length(u_Lights[i].position - fs_in.fragPos);
		float total_attenuation = u_Lights[i].attenuation.x +
							(u_Lights[i].attenuation.y * distance) +
							(u_Lights[i].attenuation.z * distance * distance);
							
							
		//vec3 scatteredLight = object_ambient_colour + u_Lights[i].colour * diffuse;
		vec3 scatteredLight = u_Lights[i].colour * diffuse;
		vec3 reflectedLight = u_Lights[i].colour * spec;
		result_colour += amb_colour + ((scatteredLight + reflectedLight)/total_attenuation);
		
	}
	
	//result_colour += object_ambient_colour;
	result_colour *= object_ambient_colour;
	return result_colour;
}





void main()
{
	//ambiency with object texture
	float ambiencyStrength = 1.0f;//0.05f;//1.0f;
	vec3 tex_colour = texture(u_Texture, fs_in.texCoords).rgb;
	
	//if performming gamma correction to shader to pervent texture contribute to be correction twice
	//as most tetxure are in sRGB space cause it to be too bright, so convert to linear space to work on. 
	if(u_GammaCorrection)
	{
		float gamma = u_Gamma;
		tex_colour = pow(tex_colour, vec3(gamma));
	}
	vec3 tex_sample_colour = (!u_DisableTex) ? tex_colour : vec3(1.0f);  //if tex is not disable use texture else plain white
	vec3 ambinent = ambiencyStrength * tex_sample_colour;
	
	//perform light calculation (Diffuse & Specular)
	//vec3 diffuse_spec = CalLightDiffuse_Spec(tex_sample_colour);
	//vec3 diffuse_spec = CalLightDiffuse_Spec(ambinent);
	vec3 diffuse_spec = vec3(0.0f);
	
	
	//CalculatePointLights
	diffuse_spec = CalculatePointLight(ambinent);
		
	FragColour = vec4(diffuse_spec, 1.0f);

	
	
	if(u_DebugScene)
	{
		if(u_DebugWcType == DEBUG_WC_MODEL_SPACE)
			FragColour = abs(fs_in.position);
		else if(u_DebugWcType == DEBUG_WC_NORMAL)
			FragColour = vec4((fs_in.normal), 1.0f);
		else if(u_DebugWcType == DEBUG_WC_MODEL_NOR)
			FragColour = vec4((fs_in.modelNor), 1.0f);
		else if(u_DebugWcType == DEBUG_WC_MODEL_ABS_NOR)
			FragColour = vec4(abs(fs_in.modelNor), 1.0f);
		else if(u_DebugWcType == DEBUG_WC_MODEL_COLOUR)
			FragColour = fs_in.colour;
		else
			FragColour = vec4(1.0f, 0.0f, 1.0f, 1.0f);//default to magenta
	}
	

	//////////////////////////////////
	//GAMMA CORRECTION
	//////////////////////////////////
	if(u_GammaCorrection)
	{
		float gamma = u_Gamma;
		FragColour.rgb = pow(FragColour.rgb, vec3(1.0f/gamma));
	}
}