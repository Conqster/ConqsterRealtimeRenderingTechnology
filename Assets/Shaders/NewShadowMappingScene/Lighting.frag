#version 420

out vec4 FragColour;

//--------------------Structs ------------------------/
struct Material
{
	vec3 diffuse;
	vec3 ambient;
	vec3 specular;
	float shinness;
};

struct DirectionalLight
{
	vec3 direction;
	bool enable;
	vec3 diffuse;
	vec3 ambient;
	vec3 specular;
};

struct PointLight
{
	vec3 position;
	bool enable;
	
	vec3 diffuse;
	float constantAtt;
	vec3 ambient;
	float linearAtt;
	vec3 specular;
	float quadraticAtt;
};


struct SpotLight
{
	vec3 position;
	bool enable;
	
	vec3 diffuse;
	float constantAtt;
	vec3 ambient;
	float linearAtt;
	vec3 specular;
	float quadraticAtt;
	
	vec3 direction;
	float innerCutoffAngle;
	float outerCutoffAngle;
	bool debug; 
};


//------------------------ Parameters ----------------------------/
in VS_OUT
{
	vec3 fragPos;
	vec3 viewPos;
	vec3 normal;
	vec4 fragPosLightSpace;
}fs_in;

uniform Material u_Material;
uniform DirectionalLight u_DirectionalLight;
uniform PointLight u_PointLight;
uniform SpotLight u_SpotLight;
uniform bool u_PhongRendering;

uniform bool u_OnlyBaseColour;

uniform bool u_FrameHasShadow;
layout(binding = 0) uniform sampler2D u_DirShadowMap;

layout(binding = 1) uniform samplerCube u_SkyboxTex;
uniform bool u_UseSkybox;

layout(binding = 2) uniform samplerCube u_PointShadowCube;
uniform float u_OmniShadowFar;


vec3 sampleOffsetDir[20] = vec3[]
(
	vec3(1, 1,  1), vec3( 1, -1,  1), vec3(-1, -1,  1), vec3(-1, 1,  1), 
	vec3(1, 1, -1), vec3( 1, -1, -1), vec3(-1, -1, -1), vec3(-1, 1, -1), 
	vec3(1, 1,  0), vec3( 1, -1,  0), vec3(-1, -1,  0), vec3(-1, 1,  0), 
	vec3(1, 0,  0), vec3(-1,  0,  1), vec3( 1,  0, -1), vec3(-1, 0, -1), 
	vec3(0, 0,  1), vec3( 0, -1,  1), vec3( 0, -1, -1), vec3( 0, 1, -1)
);

float DirShadowCalculation(vec4 shadow_coord);
float OmniShadowCalculation(vec3 light_pos, samplerCube shadow_cube, float pixel_view_dist, float far);

vec3 ComputeDirectionalLight(DirectionalLight light, Material mat, vec3 N, vec3 V);
vec3 ComputePointLight(PointLight light, vec3 N, vec3 V);

void main()
{
	//cam view direction
	vec3 V = normalize(fs_in.viewPos - fs_in.fragPos);
	vec3 N = normalize(fs_in.normal);
	
	vec3 result_colour = vec3(0.0f);
	if(u_DirectionalLight.enable)
		result_colour += ComputeDirectionalLight(u_DirectionalLight, u_Material, N, V);
	
	if(u_PointLight.enable)
		result_colour += ComputePointLight(u_PointLight, N, V);
		
	
	vec3 skybox_contribution = vec3(0.0f);
	if(u_UseSkybox)
	{
		vec3 reflection_dir = reflect(-V, N);
		result_colour += 0.3f * texture(u_SkyboxTex, reflection_dir).rgb;
	}
	
	//dither and gamma correction
	vec2 dither_coord = gl_FragCoord.xy;// / vec2(textureSize(u_Material.baseMap, 0));
	float dither = fract(sin(dot(dither_coord, vec2(12.9898, 78.233))) * 43758.5453);
	result_colour += dither * 0.003;

	
	FragColour = vec4(result_colour, 1.0f);

	if(u_OnlyBaseColour)
		FragColour = vec4(u_Material.ambient + u_Material.diffuse, 1.0f);
	//FragColour = vec4(1.0f, 0.0f, 1.0f, 1.0f);
}



float DirShadowCalculation(vec4 shadow_coord)
{
	//project texture coordinate & fecth the center sample
	vec3 p = shadow_coord.xyz / shadow_coord.w;
	
	p = p * 0.5f + 0.5f;
	
	//Using PCF
    float shadow = 0.0f;
	float bias = 0.00f;
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

float OmniShadowCalculation(vec3 light_pos, samplerCube shadow_cube, float pixel_view_dist, float far)
{
	float disk_radius = (1.0f + (pixel_view_dist / far))/ 25.0f;
	vec3 vecLF = fs_in.fragPos - light_pos; //fragPos vec to light position 
	float curr_depth = length(vecLF);
	
	float shadow = 0.0f;
	int samples = 20; //20 samplers available
	float bias = 0.0f;
	bias = 0.03f;//0.05f;
	for(int i = 0; i < samples; ++i)
	{
		float closest_depth = texture(shadow_cube, vecLF + sampleOffsetDir[i] * 0.01f/*0.005f*/, disk_radius).r;
		closest_depth *= far;
		if(curr_depth - bias > closest_depth)
			shadow += 1;
	}
	shadow /= float(samples);
	
	return shadow;
}


vec3 ComputeDirectionalLight(DirectionalLight light, Material mat, vec3 N, vec3 V)
{
	vec3 ambient = 0.2f * light.ambient * mat.ambient;
	
	//diffuse component
	vec3 Ld = normalize(light.direction);//light direction
	//Lambert cosine law
	float factor = max(dot(N, Ld), 0.0f);
	vec3 diffuse = light.diffuse * mat.diffuse * factor;
	
	float specularity = 0.0f;
	//specular component (Blinn-Phong)
	if(!u_PhongRendering)
	{
		//halfway 
		vec3 H = normalize(Ld + V);
		specularity = pow(max(dot(N, H), 0.0f), mat.shinness);
	}
	else
	{
		specularity = pow(max(dot(N, Ld), 0.0f), mat.shinness);
	}
	vec3 specular = light.specular * mat.specular * specularity;
	
	float shadow = 0.0f;
	if(u_FrameHasShadow)
		shadow = DirShadowCalculation(fs_in.fragPosLightSpace);
	

	return ambient + ((1.0f - shadow) * (diffuse + specular));
}


vec3 ComputePointLight(PointLight light, vec3 N, vec3 V)
{
	vec3 ambient = 0.3f * light.ambient * u_Material.ambient;
	
	//diffuse component
	vec3 Ld = normalize(light.position - fs_in.fragPos);//light direction fragPos from light
	//Lambert cosine law
	float factor = max(dot(N, Ld), 0.0f);
	vec3 diffuse = light.diffuse * u_Material.diffuse * factor;
	
	
	float specularity = 0.0f;
	//specular component (Blinn-Phong)
	if(!u_PhongRendering)
	{
		//halfway 
		vec3 H = normalize(Ld + V);
		specularity = pow(max(dot(N, H), 0.0f), u_Material.shinness);
	}
	else
	{
		specularity = pow(max(dot(N, Ld), 0.0f), u_Material.shinness);
	}
	vec3 specular = light.specular * u_Material.specular * specularity;
	
	
	//attenuation
	float r = length(light.position - fs_in.fragPos);
	float attenuation = light.constantAtt + 
						(light.linearAtt * r) +
						(light.quadraticAtt * r * r);
	
	float shadow = 0.0f;
	if(u_FrameHasShadow)
	{
		float pixel_view_dist = length(fs_in.viewPos - fs_in.fragPos);
		shadow = OmniShadowCalculation(light.position, u_PointShadowCube, pixel_view_dist, u_OmniShadowFar);
	}
	
	return ambient + ((1.0f - shadow) * (diffuse + specular))/attenuation;					
	return (ambient + diffuse + specular)/attenuation;
}
