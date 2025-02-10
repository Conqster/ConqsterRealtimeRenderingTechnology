#version 400

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
}fs_in;

uniform Material u_Material;
uniform DirectionalLight u_DirectionalLight;
uniform PointLight u_PointLight;
uniform SpotLight u_SpotLight;
uniform bool u_PhongRendering;

uniform bool u_OnlyBaseColour;

uniform samplerCube u_SkyboxTex;
uniform bool u_UseSkybox;

vec3 ComputeDirectionalLight(DirectionalLight light, Material mat, vec3 N, vec3 V);
vec3 ComputePointLight(PointLight light, vec3 N, vec3 V);
vec3 ComputeSpotLight(SpotLight light, vec3 N, vec3 V);

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
		
	if(u_SpotLight.enable)
		result_colour += ComputeSpotLight(u_SpotLight, N, V);
	
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
	
	vec3 gamma_corrected = pow(result_colour, vec3(1.0f/2.2f));
	
	FragColour = vec4(result_colour, 1.0f);
	//FragColour = vec4(gamma_corrected, 1.0f);

	if(u_OnlyBaseColour)
		FragColour = vec4(u_Material.ambient + u_Material.diffuse, 1.0f);
	//FragColour = vec4(1.0f, 0.0f, 1.0f, 1.0f);
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
	
	
	return ambient + diffuse + specular;
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
						
	return (ambient + diffuse + specular)/attenuation;
}


vec3 ComputeSpotLight(SpotLight light, vec3 N, vec3 V)
{
	vec3 ambient = 0.2f * light.ambient * u_Material.ambient;
	
	//diffuse component
	//Ld is the fragment / pixel direction to light not the light direction
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
						
	float theta = dot(Ld, normalize(light.direction));

	
	float ratio = (theta - light.innerCutoffAngle) / (light.outerCutoffAngle - light.innerCutoffAngle);
	float intensity = smoothstep(1.0f, 0.0f, ratio);
	
	//for debugging
	if(light.debug)
	{
		vec3 debug = mix(vec3(intensity, intensity, 0.0f), vec3(0.0f,intensity, 0.0f), ratio);
		return debug/attenuation;
	}
	
	
	return (ambient + (diffuse + specular) * intensity)/attenuation;
}