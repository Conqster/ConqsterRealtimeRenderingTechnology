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
//CONSTANTS
///////////////////////////////
const int MAX_POINT_LIGHTS = 7;


///////////////////////////////
//UNIFORMS
///////////////////////////////


//-------------------------Shadow Map Sampler---------------------------
uniform sampler2D u_ShadowMap;
uniform samplerCube u_PointShadowCubes[MAX_POINT_LIGHTS];


uniform sampler2D u_Texture;
uniform float u_FarPlane; // all point lights is uniform for now
uniform int u_ShadowSampleType;
uniform Light u_Lights[MAX_POINT_LIGHTS];
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


//SHADOW MAP SAMPLING
#define SHADOW_PCF_SAMPLING 0  //Percentage-closer filtering
#define SHADOW_POISSON_SAMPLING 1

vec2 poissonDisk[4] = vec2[](
                      vec2(-0.94201624, -0.39906216),
                      vec2(0.94558609, -0.76890725),
                      vec2(-0.094184101, -0.92938870),
                      vec2(0.34495938, 0.29387760)
                      );
                      
            

float ShadowCalculation(vec4 fragPosLightSpace)
{
	vec3 projCoords = fragPosLightSpace.xyz / fragPosLightSpace.w;
    
        if(projCoords.z > 1.0f)
            return 0.0f;
	
	projCoords = projCoords * 0.5f + 0.5f;
	float closestDepth = texture(u_ShadowMap, projCoords.xy).r;
	float currentDepth = projCoords.z;
	float bias = 0.005f;
	//bias = max(0.05f * (1.0f - dot(normalize(fs_in.normal), u_Lights[5].direction)), 0.005f);
	bias = max(0.007f * (1.0f - dot(normalize(fs_in.normal), u_Lights[5].direction)), 0.001f);
	//bias = 0.0f;
	float shadow = currentDepth - bias > closestDepth ? 1.0f : 0.0f;
    
    shadow = 0.0f;
    //PCF
        vec2 texelSize = 1.0f / textureSize(u_ShadowMap, 0);
        for(int x = -1; x <= 1; ++x)
        {
            for(int y = -1; y <= 1; ++y)
            {
                float pcfDepth = texture(u_ShadowMap, projCoords.xy + vec2(x, y) * texelSize).r;
                shadow += currentDepth - bias > pcfDepth ? 1.0f : 0.0f;
            }
        }
        shadow /= 20.0f;
    float visibility = 1.0f;
    if(u_ShadowSampleType == SHADOW_POISSON_SAMPLING)
    {
        for(int i = 0; i < 4; i++)
        {
            if(texture(u_ShadowMap, projCoords.xy + poissonDisk[i]/700.0f).z < projCoords.z - bias)
                visibility -= 0.2f;
                
            //visibility -= 0.2f * (1.0f - texture(u_ShadowMap, vec3(projCoords.xy + poissonDisk[i]/700.0f, (projCoords.z - bias)/projCoords.w)));
        }
        return visibility * shadow;
    }

	
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


float PointLightShadowCal(int lightIdx, vec3 frag_in_pos)
{
    //vec3 vfl = fragPos - light.position; 
    vec3 vfl = frag_in_pos - u_Lights[lightIdx].position; 
    
    //float closestDepth = texture(u_PointShadowMap, vfl).r;
    float closestDepth = texture(u_PointShadowCubes[lightIdx], vfl).r;
    //[0;1] => [0;far_plane]
    //probably remap during depth samplign
    closestDepth *= u_FarPlane; 
    
    float currentDepth = length(vfl);
    
    float bias = 0.005f;
	//bias = max(0.007f * (1.0f - dot(normalize(fs_in.normal), u_Lights[5].direction)), 0.001f);
    
    float shadow = currentDepth - bias > closestDepth ? 1.0f : 0.0f;
    return shadow;
}

vec3 CalculatePointLight(vec3 object_ambient_colour)
{
	vec3 result_colour = vec3(0.0f);
	
	//return CalculateDirLight(u_Lights[5]) * object_ambient_colour;
	
	for(int i = 0; i < u_LightCount; i++)
	{
		if(i > MAX_POINT_LIGHTS)
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
        
        
        float shadow = 0.0f;
        //if(i == 0)
            //shadow = PointLightShadowCal(0, fs_in.fragPos);
        
        shadow = PointLightShadowCal(i, fs_in.fragPos);


        //vec3 lighting = (amb_colour + (1.0f - shadow) * (diff_factor + spec)) * light.colour;
        vec3 lighting = (amb_colour + (1.0f - shadow) * ((diffuse + spec)/total_attenuation)) * u_Lights[i].colour;
        
        result_colour += lighting;
		//result_colour += amb_colour + ((scatteredLight + reflectedLight)/total_attenuation);
		
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
		
	FragColour = vec4(diffuse_spec, 1.0f) + vec4(0.0f, 0.0f, 1.0f, 1.0f);

	
	
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