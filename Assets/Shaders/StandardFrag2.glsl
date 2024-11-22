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

const int MAX_POINT_LIGHTS = 4 + 3;
//--------------uniform--------------/
//Model specify 
uniform Material u_Material;
uniform vec3 u_ViewPos;
//Hack 
uniform int u_PtLightCount = 0;

//Light specify
layout (std140) uniform u_LightBuffer
{
    DirectionalLight dirLight;                  //aligned
    PointLight pointLights[MAX_POINT_LIGHTS];   //aligned
};
uniform bool u_EnableSceneShadow;
uniform sampler2D u_DirShadowMap;
uniform samplerCube u_PointShadowCube;

//--------------------Environment-----------------------------------------
//struct Environment only called once if changed
layout (std140) uniform u_EnvironmentBuffer
{
	bool useSkybox;
	float skyboxInfluencity;
	float objectReflectivity; 
	vec2 alignmentPadding;
};
uniform samplerCube u_SkyboxMap;

///////////////////////////////
//FUNCTIONS DECLARATIONs
///////////////////////////////
float CalculateDirectionalShadow(vec4 shadow_coord);
float CalculatePointShadow(samplerCube depth_map, vec3 light_pos);

vec3 CalculateDirectionalLight(DirectionalLight light, vec3 base_colour, vec3 normal, vec3 view_dir, float shadow);
vec3 CalculatePointLight();


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
		float dir_shadow = CalculateDirectionalShadow(fs_in.fragPosLightSpace);
		commulated_light += CalculateDirectionalLight(dirLight, base_colour, N, V, dir_shadow);
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
	
	vec3 lighting = (ambinent + (1.0f - shadow) * (diffuse + specular)) * base_colour;
	return lighting;
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