#version 400

in vec4 v_Colour;
in vec3 v_Normals;
in vec3 v_FragPos;
in vec2 v_TexCoord;

out vec4 o_Colour;


//////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////MATERIAL////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////
struct Material
{
	sampler2D diffuseMap;
	sampler2D specularMap;
	sampler2D emissionMap;
	int shininess;
	
	bool invertSpecularMap;
	vec3 emissionColour;
	float emissionMapStrength;
	bool emit;
};



//////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////LIGHTS////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////
struct Light
{
	//vec3 position;
	
	vec3 ambient;
	vec3 diffuse;
	vec3 specular;
	
	bool use;
};

struct DirectionalLight
{
	Light lightBase;
	
	vec3 direction;
};

struct PointLight
{
	Light lightBase;
	
	vec3 position;
	
	float constant;
	float linear;
	float quadratic;
};


struct SpotLight
{
	PointLight pointLightBase;
	
	vec3 direction; 
	float innerCutoffAngle;
	float outerCutoffAngle;
};

//////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////TEST VARIABLES///////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////
//uniform float u_DebugMagnifier = 5.0f;

uniform sampler2D u_Texture;
uniform Material u_Material;
uniform Light u_light;
uniform DirectionalLight u_DirectionalLight;
uniform PointLight u_PointLights[4];
uniform SpotLight u_SpotLight;
uniform vec3 u_LightColour;
uniform bool u_IsLight;

uniform float u_GlobalAmbientStrength;
uniform float u_GlobalDiffuseStrength;
uniform float u_GlobalSpecularStrength;
uniform sampler2D u_LightMap;

uniform vec3 u_ViewPos;

uniform vec3 u_DepthFogColour;
float near = 0.1f;
float far = 150.0f;

float LinearizeDepth(float depth)
{
	float z = depth * 2.0f - 1.0f;
	return (2.0f * near * far) /(far + near - z * (far - near));
}


vec3 CalDiffuseLightDir()
{
	vec3 resultColour = vec3(0.0f);
	
	
	vec3 norm = normalize(v_Normals);
	
	//vec3 lightDir = normalize(u_light.position - v_FragPos);
	vec3 lightDir = normalize(-u_DirectionalLight.direction);
	
	float diff = max(dot(norm, lightDir), 0.0f);
	//since u_Material diffuse is no more a vec3 now a sampler 2d texColour
	//resultColour = u_light.diffuse * diff * vec3(texture(u_Material.diffuseMap, v_TexCoord));
	resultColour = u_DirectionalLight.lightBase.diffuse * diff * vec3(texture(u_Material.diffuseMap, v_TexCoord));
	return resultColour;
}


vec3 CalSpecularColour()
{
	//vec3 lightDir = normalize(u_light.position - v_FragPos);
	vec3 lightDir = normalize(-u_DirectionalLight.direction);
	vec3 viewDir = normalize(u_ViewPos - v_FragPos);
	vec3 norm = normalize(v_Normals);
	vec3 reflectDir = reflect(-lightDir, norm);
	
	float spec = pow(max(dot(viewDir, reflectDir), 0.0), u_Material.shininess);
	
	vec3 specular = vec3(1.0f);
	if(u_Material.invertSpecularMap)
		specular = u_DirectionalLight.lightBase.specular * spec *  (vec3(1.0f) - vec3(texture(u_Material.specularMap, v_TexCoord)));//u_LightColour * (spec * u_Material.specular);
		//specular = u_light.specular * spec *  (vec3(1.0f) - vec3(texture(u_Material.specularMap, v_TexCoord)));//u_LightColour * (spec * u_Material.specular);
	else
		specular = u_DirectionalLight.lightBase.specular * spec *  (vec3(texture(u_Material.specularMap, v_TexCoord)));//u_LightColour * (spec * u_Material.specular);
		//specular = u_light.specular * spec *  (vec3(texture(u_Material.specularMap, v_TexCoord)));//u_LightColour * (spec * u_Material.specular);
	return specular;
}


vec3 DirectionalLightCommulation()
{
	if(!u_DirectionalLight.lightBase.use)
		return vec3(0.0f);
	
	//since u_Material diffuse is no more a vec3 now a sampler 2d texColour
	//vec3 ambientColour = u_light.ambient * vec3(texture(u_Material.diffuseMap, v_TexCoord)) * u_GlobalAmbientStrength;
	vec3 ambientColour = u_DirectionalLight.lightBase.ambient * vec3(texture(u_Material.diffuseMap, v_TexCoord)) * u_GlobalAmbientStrength;
	
	vec3 diffuseColour = CalDiffuseLightDir() * u_GlobalDiffuseStrength;
		
	vec3 specularColour = CalSpecularColour() * u_GlobalSpecularStrength;
	
	return ambientColour + diffuseColour + specularColour;
}

vec3 PointLightCommulation(PointLight ptLight)
{
	if(!ptLight.lightBase.use)
		return vec3(0.0f);
	
	//ambient
	vec3 ambientColour = (ptLight.lightBase.ambient * texture(u_Material.diffuseMap, v_TexCoord).rgb) * u_GlobalAmbientStrength;
	
	//diffuse
	vec3 norm = normalize(v_Normals);
	vec3 lightDir = normalize(ptLight.position - v_FragPos);
	float diff = max(dot(norm, lightDir), 0.0f);
	vec3 diffuseColour = (ptLight.lightBase.diffuse * diff * vec3(texture(u_Material.diffuseMap, v_TexCoord))) * u_GlobalDiffuseStrength;
	
	//specular
	vec3 reflectDir = reflect(-lightDir, norm);
	vec3 viewDir = normalize(u_ViewPos - v_FragPos);
	float spec = pow(max(dot(viewDir, reflectDir), 0.0), u_Material.shininess);
	
	vec3 specularColour = vec3(1.0f);
	if(u_Material.invertSpecularMap)
		specularColour = ptLight.lightBase.specular * spec *  (vec3(1.0f) - vec3(texture(u_Material.specularMap, v_TexCoord)));
	else
		specularColour = ptLight.lightBase.specular * spec *  (vec3(texture(u_Material.specularMap, v_TexCoord)));
		
	specularColour *= u_GlobalSpecularStrength;
	
	float distance = length(ptLight.position - v_FragPos);
	float attenuation = 1.0f / 
						(ptLight.constant + 
						(ptLight.linear * distance) + 
						(ptLight.quadratic * distance * distance));
						
	
	
	return (ambientColour + diffuseColour + specularColour) * attenuation;
}


vec3 SpotLightCommulation()
{
	if(!u_SpotLight.pointLightBase.lightBase.use)
		return vec3(0.0f);
	
	vec3 spotLightResult = vec3(0.0f);
	
	vec3 lightDir = normalize(u_SpotLight.pointLightBase.position - v_FragPos);
	float theta = dot(lightDir, normalize(-u_SpotLight.direction));
	
	//ambient
	vec3 ambientColour = (u_SpotLight.pointLightBase.lightBase.ambient * texture(u_Material.diffuseMap, v_TexCoord).rgb) * u_GlobalAmbientStrength;
	
	//diffuse
	vec3 norm = normalize(v_Normals);
	float diff = max(dot(norm, lightDir), 0.0f);
	vec3 diffuseColour = (u_SpotLight.pointLightBase.lightBase.diffuse * diff * vec3(texture(u_Material.diffuseMap, v_TexCoord))) * u_GlobalDiffuseStrength;
	
	//specular
	vec3 reflectDir = reflect(-lightDir, norm);
	vec3 viewDir = normalize(u_ViewPos - v_FragPos);
	float spec = pow(max(dot(viewDir, reflectDir), 0.0), u_Material.shininess);
	
	vec3 specularColour = vec3(1.0f);
	if(u_Material.invertSpecularMap)
		specularColour = u_SpotLight.pointLightBase.lightBase.specular * spec *  (vec3(1.0f) - vec3(texture(u_Material.specularMap, v_TexCoord)));
	else
		specularColour = u_SpotLight.pointLightBase.lightBase.specular * spec *  (vec3(texture(u_Material.specularMap, v_TexCoord)));
		
	specularColour *= u_GlobalSpecularStrength;
	
	
	float distance = length(u_SpotLight.pointLightBase.position - v_FragPos);
	float attenuation = 1.0f / 
						(u_SpotLight.pointLightBase.constant + 
						(u_SpotLight.pointLightBase.linear * distance) + 
						(u_SpotLight.pointLightBase.quadratic * distance * distance));
						
	float epislon = u_SpotLight.innerCutoffAngle - u_SpotLight.outerCutoffAngle;
	//float intensity = clamp((theta - u_SpotLight.outerCutoffAngle) / epislon, 0.0f, 1.0f);
	float intensity = smoothstep(0.0f, 1.0f, (theta - u_SpotLight.outerCutoffAngle) /epislon);
	
	//intensity *= texture(u_LightMap, v_TexCoord).r * 50.0f;
						
	spotLightResult = (ambientColour + (diffuseColour + specularColour) * intensity) * attenuation;		
	//spotLightResult = (ambientColour + (diffuseColour + specularColour) * (intensity * vec3(0.0f, 1.0f, 0.0f))) * attenuation;		
	

	return spotLightResult;
}


void main()
{

	if(u_IsLight)
	{
		o_Colour = vec4(u_LightColour, 1.0f);
		//influence light object with light ambient, diffuse and specular colour 
		//o_Colour = vec4(u_light.ambient * u_light.diffuse * u_light.specular, 1.0f);
	}
	else
	{
		vec3 dirLight_result = DirectionalLightCommulation();
		
		
		vec3 ptLights_result = vec3(0.0f);
		for(uint i = 0; i < 4; i++)
			ptLights_result += PointLightCommulation(u_PointLights[i]);
			
		vec3 spotLight_result = SpotLightCommulation();
		
		vec3 emissionColour = vec3(0.0f);
		if(u_Material.emit)
		{
			vec3 emissionMask = step(vec3(1.0f), vec3(1.0f) - texture(u_Material.specularMap, v_TexCoord).rgb);
			emissionColour = u_Material.emissionColour * vec3(texture(u_Material.emissionMap, v_TexCoord)) * u_Material.emissionMapStrength * emissionMask;
			//if(texture(u_Material.specularMap, v_TexCoord).r == 0.0f)
				//emissionColour = u_Material.emissionColour * vec3(texture(u_Material.emissionMap, v_TexCoord)) * u_Material.emissionMapStrength;
		}			

		//depth test & fog 
		float depth = LinearizeDepth(gl_FragCoord.z) / far;
		vec3 fogColourStrength = u_DepthFogColour * depth;
		
		vec3 finalColour = dirLight_result + ptLights_result + spotLight_result + emissionColour + fogColourStrength;
		o_Colour = vec4(finalColour, 1.0f);
	}
}