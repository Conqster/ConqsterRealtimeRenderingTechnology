#version 420 core
out vec4 o_FragColour;

in vec2 vs_TexCoords;

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



layout(binding = 0) uniform sampler2D u_GBaseColour;
layout(binding = 1) uniform sampler2D u_GNormal;
layout(binding = 2) uniform sampler2D u_GPosition;
layout(binding = 3) uniform sampler2D u_GDepth_MatShinness;


const int MAX_POINT_LIGHTS = 1000;
const int MAX_POINT_LIGHT_SHADOW = 10;
//--------------uniform--------------/
uniform vec3 u_ViewPos;
uniform int u_PtLightCount = 0;

//Light specify
layout (std140) uniform u_LightBuffer
{
    DirectionalLight dirLight;                  //aligned
    PointLight pointLights[MAX_POINT_LIGHTS];   //aligned
};

///////////////////////////////
//FUNCTIONS DECLARATIONs
///////////////////////////////
vec3 CalculateDirectionalLight(DirectionalLight light, vec3 base_colour, vec3 normal, vec3 view_dir, float mat_shinnes, float shadow);
vec3 CalculatePointLights(vec3 base_colour, vec3 normal, vec3 view_dir, vec3 frag_pos, float mat_shinness);



void main()
{
	vec3 base_colour = texture(u_GBaseColour, vs_TexCoords).rgb;
	vec3 normal = texture(u_GNormal, vs_TexCoords).rgb;
	vec3 frag_pos = texture(u_GPosition, vs_TexCoords).rgb;
	float mat_shinness = texture(u_GDepth_MatShinness, vs_TexCoords).a;
	//mat_shinness = 0.0f;
	
	vec3 commulated_light = vec3(0.0f);
	vec3 view_dir = normalize(u_ViewPos - frag_pos);
	if(dirLight.enable)
	{
		//0.0f -> no shadow calculation yet.......
		commulated_light += CalculateDirectionalLight(dirLight, base_colour, normal, view_dir, mat_shinness, 0.0f);
	}
	
	//point contribution 
	commulated_light += CalculatePointLights(base_colour, normal, view_dir, frag_pos, mat_shinness);
	
	o_FragColour = vec4(commulated_light, 1.0f);
}



//--------------------Light Cal---------------------------------/
vec3 CalculateDirectionalLight(DirectionalLight light, vec3 base_colour, vec3 N, vec3 V, float mat_shinnes, float shadow)
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
	float spec_factor = pow(max(dot(N, H), 0.0f), mat_shinnes);
	//have a specular map later
	vec3 specular = light.specular * light.colour * spec_factor;
	
	vec3 lighting = (ambinent + (1.0f - shadow) * (diffuse + specular));// * base_colour;
	return lighting;
}


vec3 CalculatePointLights(vec3 base_colour, vec3 N, vec3 V, vec3 frag_pos, float mat_shinnes)
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
		vec3 Ld = normalize(pointLights[i].position - frag_pos);//light direction
		float factor = max(dot(Ld, N), 0.0f);
		//similar tp ambinent, diffuse = light diffuse intensity * light diffuse colour * object/model colour
		vec3 diffuse = pointLights[i].diffuse * pointLights[i].colour * factor * base_colour;
		
		//Specular
		vec3 H = normalize(Ld + V);//halfway view & light direction
		float spec_factor = pow(max(dot(N, H), 0.0f), mat_shinnes);
		//have a specular map later
		vec3 specular = pointLights[i].specular * pointLights[i].colour * spec_factor;
		
		//Shadow calculation 
		//0.0f -> no shadow calculation yet.......
		float shadow = 0.0f;
		//if(u_SceneAsShadow)
		//{
		//	float pixel_view_dist = length(u_ViewPos - fs_in.fragPos);
		//	shadow = CalculatePointShadow(u_PointShadowCubes[i], pointLights[i], pixel_view_dist);
		//}

		
		//attenuation
		float distance = length(pointLights[i].position - frag_pos);
		float total_attenuation = pointLights[i].attenuation.x +
								 (pointLights[i].attenuation.y * distance) +
								 (pointLights[i].attenuation.z * distance * distance);
		
		accumulated_point_light += ((ambinent + (1.0f - shadow) * (diffuse + specular)) / total_attenuation);
	}
	return accumulated_point_light;
}