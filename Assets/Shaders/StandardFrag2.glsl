#version 420

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
//--------------------Material-----------------------------------------
struct Material
{
	bool isTransparent;
	vec4 baseColour;
	
	sampler2D baseMap;
	sampler2D normalMap;
	bool useNormal;
	
	sampler2D parallaxMap;
	bool useParallax;
	float parallax;
	
	int shinness;
};


in VS_OUT
{
	vec3 fragPos;
	vec2 UVs;
	vec4 fragPosLightSpace;
	mat3 TBN;
	vec3 normal;
}fs_in;

const int MAX_POINT_LIGHTS = 10;
//--------------uniform--------------/
//Model specify 
uniform Material u_Material;
uniform vec3 u_ViewPos;
uniform bool u_SceneAsShadow = false;
//Hack 
uniform int u_PtLightCount = 0;

//Light specify
layout (std140) uniform u_LightBuffer
{
    DirectionalLight dirLight;                  //aligned
    PointLight pointLights[MAX_POINT_LIGHTS];   //aligned
};
//-------------------------Shadow Map Sampler---------------------------
uniform bool u_EnableSceneShadow;
layout(binding = 3) uniform sampler2D u_DirShadowMap;
layout(binding = 4) uniform samplerCube u_SkyboxMap;
layout(binding = 5) uniform samplerCube u_PointShadowCubes[MAX_POINT_LIGHTS];

//--------------------Environment-----------------------------------------
//struct Environment only called once if changed
layout (std140) uniform u_EnvironmentBuffer
{
	bool useSkybox;
	float skyboxInfluencity;
	float objectReflectivity; 
	vec2 alignmentPadding;
};



vec3 sampleOffsetDir[20] = vec3[]
(
	vec3(1, 1,  1), vec3( 1, -1,  1), vec3(-1, -1,  1), vec3(-1, 1,  1), 
	vec3(1, 1, -1), vec3( 1, -1, -1), vec3(-1, -1, -1), vec3(-1, 1, -1), 
	vec3(1, 1,  0), vec3( 1, -1,  0), vec3(-1, -1,  0), vec3(-1, 1,  0), 
	vec3(1, 0,  0), vec3(-1,  0,  1), vec3( 1,  0, -1), vec3(-1, 0, -1), 
	vec3(0, 0,  1), vec3( 0, -1,  1), vec3( 0, -1, -1), vec3( 0, 1, -1)
);
///////////////////////////////
//FUNCTIONS DECLARATIONs
///////////////////////////////
float CalculateDirectionalShadow(vec4 shadow_coord);
float CalculatePointShadow(samplerCube depth_map, PointLight light, float pixel_view_dist);

vec3 CalculateDirectionalLight(DirectionalLight light, vec3 base_colour, vec3 normal, vec3 view_dir, float shadow);
vec3 CalculatePointLights(vec3 base_colour, vec3 normal, vec3 view_dir);

vec3 ReflectedSkybox(vec3 vdir, vec3 nor);

void main()
{
	//does not support parallax & transparency at the moment
	vec3 base_colour = u_Material.baseColour.rgb;
	base_colour *= texture(u_Material.baseMap, fs_in.UVs).rgb;
	
	vec3 N = normalize(fs_in.normal);
	if(u_Material.useNormal)
	{
		N = texture(u_Material.normalMap, fs_in.UVs).rgb;
		N = N * 2.0 - 1.0; //[0,1] >> [-1, 1]
		N = normalize(fs_in.TBN * N);  //transform nor from tangent to world space
	}
	
	//cam - view direction
	vec3 V = normalize(u_ViewPos - fs_in.fragPos);
	
	vec3 commulated_light = vec3(0.0f);
	//directional Light 
	if(dirLight.enable)
	{
		float dir_shadow = (u_SceneAsShadow) ? CalculateDirectionalShadow(fs_in.fragPosLightSpace) : 0.0f;
		commulated_light += CalculateDirectionalLight(dirLight, base_colour, N, V, dir_shadow);
	}
	//point light contribtion 
	commulated_light += CalculatePointLights(base_colour, N, V);
	
	
	//with sky box reflection 
	if(useSkybox)
	{
		vec3 v_dir = (u_Material.useParallax) ? (V * fs_in.TBN) : (V);
		//final_colour *= ReflectedSkybox(v_dir, N);
		commulated_light += mix(vec3(0.0f), ReflectedSkybox(v_dir, N), skyboxInfluencity);
	}

	FragColour = vec4(commulated_light, 1.0f);
}


//--------------------Light Cal---------------------------------/
vec3 CalculateDirectionalLight(DirectionalLight light, vec3 base_colour, vec3 N, vec3 V, float shadow)
{
	//Ambient 
	//ambinent = light ambinent colour * object/model colour
	//for now light ambinent colour is ambinent intensity * colour;
	vec3 ambinent = light.ambinent * light.colour * base_colour;
	
	//diffuse
	vec3 Ld = normalize(dirLight.direction);//light direction
	float factor = max(dot(Ld, N), 0.0f);
	//similar tp ambinent, diffuse = light diffuse intensity * light diffuse colour * object/model colour
	vec3 diffuse = light.diffuse * light.colour * factor * base_colour;
	
	//Specular
	vec3 H = normalize(Ld + V);//halfway view & light direction
	float spec_factor = pow(max(dot(N, H), 0.0f), u_Material.shinness);
	//have a specular map later
	vec3 specular = light.specular * light.colour * spec_factor;
	
	vec3 lighting = (ambinent + (1.0f - shadow) * (diffuse + specular));// * base_colour;
	return lighting;
}

vec3 CalculatePointLights(vec3 base_colour, vec3 N, vec3 V)
{
	vec3 accumulated_point_light = vec3(0.0f);
	
	//to pervent overflow 
	int light_count = (u_PtLightCount < MAX_POINT_LIGHTS) ? u_PtLightCount : MAX_POINT_LIGHTS;

	for(int i = 0; i < light_count; i++)
	{
		if(!pointLights[i].enable)
		continue;
		
		//Ambient 
		//ambinent = light ambinent colour * object/model colour
		//for now light ambinent colour is ambinent intensity * colour;
		vec3 ambinent = pointLights[i].ambinent * pointLights[i].colour * base_colour;
		
		//diffuse
		vec3 Ld = normalize(pointLights[i].position - fs_in.fragPos);//light direction
		float factor = max(dot(Ld, N), 0.0f);
		//similar tp ambinent, diffuse = light diffuse intensity * light diffuse colour * object/model colour
		vec3 diffuse = pointLights[i].diffuse * pointLights[i].colour * factor * base_colour;
		
		//Specular
		vec3 H = normalize(Ld + V);//halfway view & light direction
		float spec_factor = pow(max(dot(N, H), 0.0f), u_Material.shinness);
		//have a specular map later
		vec3 specular = pointLights[i].specular * pointLights[i].colour * spec_factor;
		
		//Shadow calculation 
		float shadow = 0.0f;
		if(u_SceneAsShadow)
		{
			float pixel_view_dist = length(u_ViewPos - fs_in.fragPos);
			shadow = CalculatePointShadow(u_PointShadowCubes[i], pointLights[i], pixel_view_dist);
		}

		
		//attenuation
		float distance = length(pointLights[i].position - fs_in.fragPos);
		float total_attenuation = pointLights[i].attenuation.x +
								 (pointLights[i].attenuation.y * distance) +
								 (pointLights[i].attenuation.z * distance * distance);
		
		accumulated_point_light += ((ambinent + (1.0f - shadow) * (diffuse + specular)) / total_attenuation);
	}
	return accumulated_point_light;
}

//----------------------------------Light Shadow---------------------------/
float CalculateDirectionalShadow(vec4 shadow_coord)
{
	//project texture coordinate & fecth the center sample
	vec3 p = shadow_coord.xyz / shadow_coord.w;
	
	p = p * 0.5f + 0.5f;
	
	//Using PCF
    float shadow = 0.0f;
	float bias = 0.0f;
    vec2 texelSize = 1.0f / textureSize(u_DirShadowMap, 0);
    for(int x = -1; x <= 1; ++x)
    {
        for(int y = -1; y <= 1; ++y)
        {
            float pcfDepth = texture(u_DirShadowMap, p.xy + vec2(x, y) * texelSize).r;
            shadow += p.z - bias > pcfDepth ? 1.0f : 0.0f;
        }
    }
    shadow /= 10.0f;
    
    return shadow;
}


float CalculatePointShadow(samplerCube depth_map, PointLight light, float pixel_view_dist)
{
	float disk_radius = (1.0f + (pixel_view_dist / light.far))/ 25.0f;
	vec3 vecLF = fs_in.fragPos - light.position; //fragPos vec to light position 
	float curr_depth = length(vecLF);
	
	float shadow = 0.0f;
	int samples = 20; //20 samplers available
	float bias = 0.0f;
	bias = 0.05f;
	for(int i = 0; i < samples; ++i)
	{
		float closest_depth = texture(depth_map, vecLF + sampleOffsetDir[i] * 0.1f, disk_radius).r;
		closest_depth *= light.far;
		if(curr_depth - bias > closest_depth)
			shadow += 1;
	}
	shadow /= float(samples);
	
	return shadow;
}


vec3 ReflectedSkybox(vec3 vdir, vec3 nor)
{
	vec3 R = reflect(-vdir, nor);
	
	//return (texture(u_SkyboxMap, R).rgb) * skyIntensity;
	//return (texture(u_SkyboxMap, R).rgb);
	return (texture(u_SkyboxMap, R).rgb) * objectReflectivity;
}