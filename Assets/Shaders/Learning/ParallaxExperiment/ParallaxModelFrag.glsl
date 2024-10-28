#version 400

//--------------attributes--------------/
out vec4 FragColour; 

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

in VS_OUT
{
	vec3 fragPos;
	vec2 UVs;
	vec4 fragPosLightSpace;
	mat3 TBN;
	vec3 normal;
}fs_in;

const int MAX_POINT_LIGHTS = 5;
//--------------uniform--------------/
//Model specify 
uniform sampler2D u_Texture;
uniform sampler2D u_NormalMap;
uniform bool u_UseNorMap;
uniform float u_ModelShinness;

uniform vec3 u_ViewPos;

//Light specify
layout (std140) uniform u_LightBuffer
{
    DirectionalLight dirLight;                  //aligned
    PointLight pointLights[MAX_POINT_LIGHTS];   //aligned
};
uniform sampler2D u_DirShadowMap;
uniform samplerCube u_PointShadowCube;

///////////////////////////////
//FUNCTIONS DECLARATIONs
///////////////////////////////
float DirShadowCalculation(vec3 N, vec4 frag_pos_lightSpace);
float PointLightShadowCal(int lightIdx);

vec3 CalDirLight(vec3 N, vec3 V, vec3 base_colour);
vec3 CalPointLight(vec3 N, vec3 V, vec3 base_colour);


void main()
{
	vec3 N = normalize(fs_in.normal);
	if(u_UseNorMap)
	{
		N = texture(u_NormalMap, fs_in.UVs).rgb;
		N = N * 2.0 - 1.0; //[0,1] >> [-1, 1]
		N = normalize(fs_in.TBN * N);  //transform nor from tangent to world space
	}

	
	
	vec3 base_colour = texture(u_Texture, fs_in.UVs).rgb;
	float ambient = 0.5f;
	base_colour *= ambient;
	
	//cam - view direction
	vec3 V = normalize(u_ViewPos - fs_in.fragPos);
	
	//directional light influence
	vec3 final_colour = CalDirLight(N, V, base_colour);
	final_colour += CalPointLight(N, V, base_colour);
	
	FragColour = vec4(final_colour, 1.0f);
}



//--------------------Light Cal---------------------------------/
vec3 CalDirLight(vec3 N, vec3 V, vec3 base_colour)
{
	vec3 result = vec3(0.0f);
	
	if(dirLight.enable)
	{
		vec3 Ld = normalize(dirLight.direction);//light direction
		float factor = max(dot(N, Ld), 0.0f);
		vec3 H = normalize(Ld + V);//halfway view & light direction
		
		//diffuse
		vec3 diffuse = dirLight.ambinent * dirLight.colour;
		
		
		//specular
		float specular = pow(max(dot(N, H), 0.0f), u_ModelShinness);
		
		float shadow = DirShadowCalculation(N, fs_in.fragPosLightSpace);
		vec3 ambient_light = (base_colour + diffuse) * factor;
		vec3 highlight = dirLight.colour * dirLight.specular * specular;
		result = (1.0 - shadow) * (ambient_light + highlight);
	}
	return result;
}

vec3 CalPointLight(vec3 N, vec3 V, vec3 base_colour)
{
	vec3 result = vec3(0.0f);
	
	//only one light for now
	for(int i = 0; i < MAX_POINT_LIGHTS; i++)
	{
		if(!pointLights[i].enable)
			continue;
		
		vec3 Ld = normalize(pointLights[i].position - fs_in.fragPos);//light direction
		float factor = max(dot(N, Ld), 0.0f);
		vec3 H = normalize(Ld + V);//halfway view & light direction
		
		//diffuse
		vec3 diffuse = pointLights[i].ambinent * pointLights[i].colour;
		
		
		//specular
		float specular = pow(max(dot(N, H), 0.0f), u_ModelShinness);
		
		//attenuation
		float distance = length(pointLights[i].position - fs_in.fragPos);
		float total_attenuation = pointLights[i].attenuation.x +
								 (pointLights[i].attenuation.y * distance) +
								 (pointLights[i].attenuation.z * distance * distance);
								 
		float shadow = (i != 0) ? PointLightShadowCal(0) : 0.0f;						  
		//Quick hack 
		vec3 ambient_light = (diffuse + base_colour) * factor;
		vec3 highlight = pointLights[i].colour * pointLights[i].specular * specular;
		result = ((1.0 - shadow) * (ambient_light + highlight))/total_attenuation;
	}
	return result;
}



//----------------------------------Light Shadow---------------------------/
float DirShadowCalculation(vec3 N, vec4 frag_pos_lightSpace)
{
	vec3 projCoords = frag_pos_lightSpace.xyz / frag_pos_lightSpace.w;
	
	if(projCoords.z > 1.0f)
		result vec3(0.0f);
		
	projCoords = projCoords * 0.5f + 0.5f;
	float closestDepth = texture(u_DirShadowMap, projCoords.xy).r;
	float currentDepth = projCoords.z;
	vec3 Ld = normalize(dirLight.direction);//light direction
	float bias = max(0.007f * (1.0f - dot(N, Ld)), 0.001f);
    float shadow = currentDepth - bias > closestDepth ? 1.0f : 0.0f;
    
	//Using PCF
    shadow = 0.0f;
    vec2 texelSize = 1.0f / textureSize(u_DirShadowMap, 0);
    for(int x = -1; x <= 1; ++x)
    {
        for(int y = -1; y <= 1; ++y)
        {
            float pcfDepth = texture(u_DirShadowMap, projCoords.xy + vec2(x, y) * texelSize).r;
            shadow += currentDepth - bias > pcfDepth ? 1.0f : 0.0f;
        }
    }
    shadow /= 20.0f;
    
    return shadow;
}


float PointLightShadowCal(int idx)
{
	vec3 Ld = fs_in.fragPos - pointLights[idx].position;
	float closestDepth = texture(u_PointShadowCube, Ld).r;
	//[0;1]>>[0;far_plane]
	closestDepth *= pointLights[idx].far;
	
	float curr_depth = length(Ld);
	float bias = 0.005f;
	float shadow = curr_depth - bias > closestDepth ? 1.0f : 0.0f;
	return shadow;
}