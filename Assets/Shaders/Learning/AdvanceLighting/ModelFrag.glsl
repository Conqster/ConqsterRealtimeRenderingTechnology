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



//////////////////////////////
//LIGHTS
//////////////////////////////
//--------------------Point Light------------------------------
struct PointLight
{
    //Description
    //for now to keep it simple 
    //ambinent, diffuse & specular only have intensity not colour(vec3)
    //float constantAttenuation;
	//float linearAttenuation;
	//float quadraticAttenuation;
	//TO-DO: Stored all attenuation contiguously in memory for easy access
	//		Probably change to a vec3 later
    
    vec3 colour;                // 12 bytes r 4
    bool enable;                // << 4 
    vec3 position;              // 12 bytes r 4
    float ambinent;             // << 4
    vec3 attenuation;           // 12 bytes r 4
    float diffuse;              // << 4
    float specular;             // 4 bytes r 12
    float far;                  // << 4 r 8
    vec2 alignmentPadding;      // << 8
};
//--------------------Directional Light----------------------
struct DirectionalLight
{
    vec3 colour;                // 12 bytes r 4
    bool enable;                // << 4
    vec3 direction;             //12 bytes r 4
    float ambinent;             // << 4
    float diffuse;              // 4 bytes r 12
    float specular;             // << 4 r 8
    vec2 alignmentPadding;      // << 8
};

///////////////////////////////
//CONSTANTS
///////////////////////////////
const int MAX_POINT_LIGHTS = 5;


///////////////////////////////
//UNIFORMS
///////////////////////////////

//------------------------Object/Model Specific-----------
uniform sampler2D u_Texture;

//--------------------Uniform Buffers Layout--------------
layout (std140) uniform u_LightBuffer
{
    DirectionalLight dirLight;                  //aligned
    PointLight pointLights[MAX_POINT_LIGHTS];   //aligned
    
    //int scenePointLightCount;                   // 4 bytes n 12
    //float alignmentPadding[3];                  // <<12
};


//-------------------------Shadow Map Sampler---------------------------
uniform sampler2D u_ShadowMap;
uniform samplerCube u_PointShadowCubes[MAX_POINT_LIGHTS];
  // PointLight pointLights[MAX_POINT_LIGHTS];   //aligned

//------------------------------Utilites----------------------------------
uniform vec3 u_ViewPos;                                     //move into Light buffer
uniform int u_Shininess;                                    //move into light buffer
uniform bool u_Blinn_Phong;
uniform bool u_GammaCorrection;
uniform float u_Gamma;


//------------------------------Debugger----------------------------------
//////////////////////////////Move out later
uniform bool u_DebugScene;
uniform bool u_DisableTex;
uniform int u_DebugWcType;

#define DEBUG_WC_MODEL_SPACE 0
#define DEBUG_WC_NORMAL 1
#define DEBUG_WC_MODEL_NOR 2
#define DEBUG_WC_MODEL_ABS_NOR 3
#define DEBUG_WC_MODEL_COLOUR 4
#define DEBUG_DEFAULT_COLOUR 5

///////////////////////////////
//FUNCTIONS DECLARATIONs
///////////////////////////////
float DirShadowCalculation(vec4 fragLightSpace, DirectionalLight light);
float PointLightShadowCal(int lightIdx, vec3 frag_in_pos);
vec3 DirLightColInfluence(DirectionalLight light, vec3 hack_texture_col);
vec3 PointLightsColInfluence(vec3 hack_texture_col);

//--------------------------------------------Main----------------------------------------------/

void main()
{
    vec3 tex_colour = texture(u_Texture, fs_in.texCoords).rgb;
    
    
    //if performming gamma correction to shader to pervent texture contribute to be correction twice
	//as most tetxure are in sRGB space cause it to be too bright, so convert to linear space to work on. 
	if(u_GammaCorrection)
	{
		tex_colour = pow(tex_colour, vec3(u_Gamma));
	}
    
    vec3 tex_sample_colour = (!u_DisableTex) ? tex_colour : vec3(1.0f);  //if tex is not disable use texture else plain white
	
    float tex_ambiency_strength = 0.5f;
    vec3 tex_ambinent = tex_sample_colour * tex_ambiency_strength;
    
    //for now
    vec3 result_colour = DirLightColInfluence(dirLight, tex_ambinent);
    result_colour += PointLightsColInfluence(tex_ambinent);
    
    //result_colour *= tex_ambinent;
    
    
    FragColour = vec4(result_colour, 1.0f);
    
    //////////
	//For Debugging
	///////////
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
	//For GAMMA CORRECTION
	//////////////////////////////////
	if(u_GammaCorrection)
	{
		FragColour.rgb = pow(FragColour.rgb, vec3(1.0f/u_Gamma));
	}
    
}

///////////////////////////////
//FUNCTIONS DEFINATIONs
///////////////////////////////

//--------------------------------Shadow Calculation---------------------------------------------
float DirShadowCalculation(vec4 fragLightSpace, DirectionalLight light)
{
    vec3 projCoords = fragLightSpace.xyz / fragLightSpace.w;
    
    if(projCoords.z > 1.0f)
        return 0.0f;
        
    projCoords = projCoords * 0.5f + 0.5f; 
    float closestDepth = texture(u_ShadowMap, projCoords.xy).r;
	float currentDepth = projCoords.z;
    float bias = max(0.007f * (1.0f - dot(normalize(fs_in.normal), light.direction)), 0.001f);
    float shadow = currentDepth - bias > closestDepth ? 1.0f : 0.0f;
    
    //Using PCF
    shadow = 0.0f;
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
    
    return shadow;
}

float PointLightShadowCal(int lightIdx, vec3 frag_in_pos)
{
    vec3 vfl = frag_in_pos - pointLights[lightIdx].position; 
    float closestDepth = texture(u_PointShadowCubes[lightIdx], vfl).r;
    //[0;1] => [0;far_plane]
    //probably remap during depth samplign
    closestDepth *= pointLights[lightIdx].far;
    //closestDepth *= 25.0f;
    
    float currentDepth = length(vfl);
    float bias = 0.005f;
    float shadow = currentDepth - bias > closestDepth ? 1.0f : 0.0f;
    return shadow;
}


//--------------------------------Light Calculations---------------------------------------------

vec3 DirLightColInfluence(DirectionalLight light, vec3 hack_texture_col)
{

    vec3 result_colour = vec3(0.0f);
    
    
    if(light.enable)
    {
        //////////////////////
        //Ambinent
        //////////////////////
        vec3 amb_colour = light.ambinent * light.colour;
    
        //context utilities
        vec3 light_dir = normalize(light.direction);
        vec3 nor = normalize(fs_in.normal);
    
        /////////////////////
        //Diffuse
        /////////////////////
        float diff_factor = max(dot(nor, light_dir), 0.0f);
    
        //Quick hack
        //if(diff_factor <= 0.0f)
            //return amb_colour + (light.colour * diff_factor);
        
        
        ///////////////
        //Speculat highlight
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
    
        vec3 scatteredLight = light.colour * light.diffuse * diff_factor;
        vec3 reflectedLight = light.colour * light.specular *spec;
    
        float shadow = DirShadowCalculation(fs_in.fragPosLightSpace, light);
        //shadow = 0.0f;
        vec3 lighting = amb_colour + (1.0f - shadow) *  ((scatteredLight + reflectedLight) );
    
        //amb_colour already has light colour 
        //scatteredLight already has light colour
        //reflectedlight already has light colour
        //Amb * scatteredLight needs model texture colour
        
        amb_colour *= hack_texture_col;
        scatteredLight *= hack_texture_col;
        
        result_colour = amb_colour + ((1.0f - shadow) * (scatteredLight + reflectedLight));
    }
    
    
        
    return result_colour;
}

//all light are registered but influence is based on the count available in scenePointLightCount
//Later have isenable
vec3 PointLightsColInfluence(vec3 hack_texture_col)
{
    vec3 result_colour = vec3(0.0f);
    
    
    
    //for(int i = 0; i < scenePointLightCount; i++)
    for(int i = 0; i < MAX_POINT_LIGHTS; i++)
    {

        if(!pointLights[i].enable)   
            continue;
        
        ///////////
		//Ambinent
		///////////
		vec3 amb_colour = pointLights[i].ambinent * pointLights[i].colour;
        
        
         //context utilities
        vec3 light_dir = normalize(pointLights[i].position - fs_in.fragPos);
        vec3 nor = normalize(fs_in.normal);
        
        
        ///////////
		//Diffuse
		///////////
		 float diff  = max(dot(light_dir, nor), 0.0f);
        
        
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
                
        
        ///////////////
        //Attenuation
		///////////////
		float distance = length(pointLights[i].position - fs_in.fragPos);
		float total_attenuation = pointLights[i].attenuation.x +
							(pointLights[i].attenuation.y * distance) +
							(pointLights[i].attenuation.z * distance * distance);
							
        diff *= pointLights[i].diffuse;
        spec *= pointLights[i].specular;
		vec3 scatteredLight = pointLights[i].colour * diff;
		vec3 reflectedLight = pointLights[i].colour * spec;
        
        ///////////////
        //Shadow Factor
		///////////////
        float shadow = 0.0f;
        shadow = PointLightShadowCal(i, fs_in.fragPos);
        
        vec3 lightingInfluence = (amb_colour + (1.0f - shadow) * ((diff + spec)/total_attenuation)) * pointLights[i].colour;
        //lightingInfluence = amb_colour + (1.0f - shadow) *  ((scatteredLight + reflectedLight)/total_attenuation );// * pointLights[i].colour;
        
        //amb_colour already has light colour 
        //scatteredLight already has light colour
        //reflectedlight already has light colour
        //Amb * scatteredLight needs model texture colour
        
        amb_colour *= hack_texture_col;
        scatteredLight *= hack_texture_col;
        
        lightingInfluence = amb_colour + ((1.0f - shadow) * ((scatteredLight + reflectedLight)/total_attenuation));
        
        result_colour += lightingInfluence;
    }
    
    return result_colour;
}












