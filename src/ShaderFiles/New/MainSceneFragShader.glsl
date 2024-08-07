#version 400

in vec4 v_Colour;
in vec3 v_Normals;
in vec3 v_FragPos;
in vec2 v_TexCoord;

out vec4 o_Colour;

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
	
	//float constant;	
	//float linear;	
	//float quadratic;	
	//TO-DO: Stored all attenuation contiguously in memory for easy access
	//	Probably change to a vec3 later
	vec3 attenuation;
};


struct SpotLight
{
	PointLight pointLightBase;
	
	vec3 direction;
	float innerCutoffAngle;
	float outerCutoffAngle;
};



uniform sampler2D u_Texture;
uniform vec3 u_ViewPos;

uniform int u_ObjectSpecularExponent;


uniform DirectionalLight u_DirectionalLight;

const int MAX_POINT_LIGHT = 5;
uniform int u_PointLightCount = 0;
uniform PointLight u_PointLights[MAX_POINT_LIGHT];

const int MAX_SPOT_LIGHT = 5;
uniform int u_SpotLightCount = 0;
uniform SpotLight u_SpotLights[MAX_SPOT_LIGHT];


uniform float u_GlobalAmbientStrength;
uniform float u_GlobalDiffuseStrength;
uniform float u_GlobalSpecularStrength;

uniform bool u_DebugMode;

//////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////FUNCTIONS///////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////

vec3 CalDiffuseLightDir()
{
	vec3 resultColour = vec3(0.0f);
	
	
	vec3 norm = normalize(v_Normals);
	
	//vec3 lightDir = normalize(u_light.position - v_FragPos);
	vec3 lightDir = normalize(u_DirectionalLight.direction);
	
	float diff = max(dot(norm, lightDir), 0.0f);
	//since u_Material diffuse is no more a vec3 now a sampler 2d texColour
	//resultColour = u_DirectionalLight.lightBase.diffuse * diff * vec3(texture(u_Material.diffuseMap, v_TexCoord));
	resultColour = u_DirectionalLight.lightBase.diffuse * diff * texture(u_Texture, v_TexCoord).rgb;
	return resultColour;
}


vec3 CalSpecularColour()
{
	//vec3 lightDir = normalize(u_light.position - v_FragPos);
	vec3 lightDir = normalize(u_DirectionalLight.direction);
	vec3 viewDir = normalize(u_ViewPos - v_FragPos);
	vec3 norm = normalize(v_Normals);
	vec3 reflectDir = reflect(-lightDir, norm);
	
	float spec = pow(max(dot(viewDir, reflectDir), 0.0), u_ObjectSpecularExponent);
	
	vec3 specular = vec3(1.0f);
	specular = u_DirectionalLight.lightBase.specular * spec;
	return specular;
}

//////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////DIRECTIONALLIGHT///////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////


vec3 CalculateDirectionalLight()
{
	if(!u_DirectionalLight.lightBase.use)
		return vec3(0.0f);
	
	//since u_Material diffuse is no more a vec3 now a sampler 2d texColour
	//vec3 ambientColour = u_light.ambient * vec3(texture(u_Material.diffuseMap, v_TexCoord)) * u_GlobalAmbientStrength;
	//vec3 ambientColour = u_DirectionalLight.lightBase.ambient * vec3(texture(u_Material.diffuseMap, v_TexCoord)) * u_GlobalAmbientStrength;
	vec3 ambientColour = u_DirectionalLight.lightBase.ambient * u_GlobalAmbientStrength * texture(u_Texture, v_TexCoord).rgb;
	
	vec3 diffuseColour = CalDiffuseLightDir() * u_GlobalDiffuseStrength;
		
	vec3 specularColour = CalSpecularColour() * u_GlobalSpecularStrength;
	
	return ambientColour + diffuseColour + specularColour;
}


//////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////POINTLIGHT///////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////


vec3 CalculatePointlight(PointLight ptLight)
{
	if(!ptLight.lightBase.use)
		return vec3(0.0f);
			
	//ambient
	//vec3 ambientColour = (ptLight.lightBase.ambient * texture(u_Material.diffuseMap, v_TexCoord).rgb) * u_GlobalAmbientStrength;
	vec3 ambientColour = (ptLight.lightBase.ambient * texture(u_Texture, v_TexCoord).rgb) * u_GlobalAmbientStrength;
	
	//diffuse
	vec3 norm = normalize(v_Normals);
	vec3 lightDir = normalize(ptLight.position - v_FragPos);
	float diff = max(dot(norm, lightDir), 0.0f);
	//vec3 diffuseColour = (ptLight.lightBase.diffuse * diff * vec3(texture(u_Material.diffuseMap, v_TexCoord))) * u_GlobalDiffuseStrength;
	vec3 diffuseColour = (ptLight.lightBase.diffuse * diff * vec3(texture(u_Texture, v_TexCoord))) * u_GlobalDiffuseStrength;
	
	//specular
	vec3 reflectDir = reflect(-lightDir, norm);
	vec3 viewDir = normalize(u_ViewPos - v_FragPos);
	float spec = pow(max(dot(viewDir, reflectDir), 0.0), u_ObjectSpecularExponent);
	
	vec3 specularColour = vec3(1.0f);
	
	specularColour = ptLight.lightBase.specular * spec * u_GlobalSpecularStrength;
	
	float distance = length(ptLight.position - v_FragPos);
	float attenuation = 1.0f / 
						(ptLight.attenuation.x + 
						(ptLight.attenuation.y * distance) + 
						(ptLight.attenuation.z * distance * distance));
						
	
	
	return (ambientColour + diffuseColour + specularColour) * attenuation;
}

//////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////SPOTLIGHT///////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////


vec3 CalculateSpotLights(SpotLight sp_light)
{
	if(!sp_light.pointLightBase.lightBase.use)
		return vec3(0.0f);
	
	
	vec3 spotLightResult = vec3(0.0f);
	
	vec3 lightDir = normalize(sp_light.pointLightBase.position - v_FragPos);

	
	//ambient
	vec3 ambientColour = (sp_light.pointLightBase.lightBase.ambient * texture(u_Texture, v_TexCoord).rgb) * u_GlobalAmbientStrength;
	
	//diffuse
	vec3 norm = normalize(v_Normals);
	float diff = max(dot(norm, lightDir), 0.0f);
	vec3 diffuseColour = (sp_light.pointLightBase.lightBase.diffuse * diff * vec3(texture(u_Texture, v_TexCoord))) * u_GlobalDiffuseStrength;
	
	//specular
	vec3 reflectDir = reflect(-lightDir, norm);
	vec3 viewDir = normalize(u_ViewPos - v_FragPos);
	float spec = pow(max(dot(viewDir, reflectDir), 0.0), u_ObjectSpecularExponent);
	
	vec3 specularColour = sp_light.pointLightBase.lightBase.specular * spec * u_GlobalSpecularStrength;
	
	
	float distance = length(sp_light.pointLightBase.position - v_FragPos);
	float attenuation = 1.0f / 
						(sp_light.pointLightBase.attenuation.x + 
						(sp_light.pointLightBase.attenuation.y * distance) + 
						(sp_light.pointLightBase.attenuation.z * distance * distance));
				
	float theta = dot(lightDir, normalize(-sp_light.direction));
	float epislon = sp_light.innerCutoffAngle - sp_light.outerCutoffAngle;
	float intensity = smoothstep(0.0f, 1.0f, (theta - sp_light.outerCutoffAngle) /epislon);
	
	return (ambientColour + (diffuseColour + specularColour) * intensity) * attenuation;
}


void main()
{
	if(u_DebugMode)
	{
		o_Colour = v_Colour;
		return;
	}
	

	
	vec3 finalColour = vec3(0.0f);
	finalColour += CalculateDirectionalLight();
	
	int temp_count = u_PointLightCount;
	if(u_PointLightCount > MAX_POINT_LIGHT)
		temp_count = MAX_POINT_LIGHT;
		
	for(int i = 0; i < temp_count; i++)
		finalColour += CalculatePointlight(u_PointLights[i]);
		
	temp_count = u_SpotLightCount;
	if(u_SpotLightCount > MAX_SPOT_LIGHT)
		temp_count = MAX_SPOT_LIGHT;
		
	for(int i = 0; i < temp_count; i++)
		finalColour += CalculateSpotLights(u_SpotLights[i]);
		

	o_Colour = vec4(finalColour, 1.0f);
}