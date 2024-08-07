#version 400

in vec2 v_TexCoords;
in vec3 v_Normals;
in vec3 v_FragPos;

out vec4 o_FragColour;

struct Material
{
	sampler2D texture_diffuse1;
	sampler2D texture_diffuse2;
	sampler2D texture_diffuse3;
	sampler2D texture_specular1;
	sampler2D texture_specular2;
	sampler2D texture_emissive1;
	int shininess;
	float emissionIntensity;
};
uniform Material u_Material;

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

uniform float u_GlobalAmbientStrength;
uniform float u_GlobalDiffuseStrength;
uniform float u_GlobalSpecularStrength;
uniform DirectionalLight u_DirectionalLight;
uniform vec3 u_ViewPos;




vec3 CalDiffuseLightDir()
{
	vec3 resultColour = vec3(0.0f);
	
	
	vec3 norm = normalize(v_Normals);
	
	//vec3 lightDir = normalize(u_light.position - v_FragPos);
	vec3 lightDir = normalize(-u_DirectionalLight.direction);
	
	float diff = max(dot(norm, lightDir), 0.0f);
	//since u_Material diffuse is no more a vec3 now a sampler 2d texColour
	//resultColour = u_light.diffuse * diff * vec3(texture(u_Material.texture_diffuse1, v_TexCoords));
	resultColour = u_DirectionalLight.lightBase.diffuse * diff * vec3(texture(u_Material.texture_diffuse1, v_TexCoords));
	
	//multiple Diffuse map
	//resultColour += u_DirectionalLight.lightBase.diffuse * diff * vec3(texture(u_Material.texture_diffuse2, v_TexCoords));
	//resultColour += u_DirectionalLight.lightBase.diffuse * diff * vec3(texture(u_Material.texture_diffuse3, v_TexCoords));

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
	specular = u_DirectionalLight.lightBase.specular * spec *  (vec3(texture(u_Material.texture_specular1, v_TexCoords)));//u_LightColour * (spec * u_Material.specular);
	//specular += u_DirectionalLight.lightBase.specular * spec *  (vec3(texture(u_Material.texture_specular2, v_TexCoords)));//u_LightColour * (spec * u_Material.specular);
	return specular;
}



vec3 DirectionalLightCommulation()
{
	if(!u_DirectionalLight.lightBase.use)
		return vec3(0.0f);
	
	//since u_Material diffuse is no more a vec3 now a sampler 2d texColour
	//vec3 ambientColour = u_light.ambient * vec3(texture(u_Material.texture_diffuse1, v_TexCoords)) * u_GlobalAmbientStrength;
	vec3 ambientColour = u_DirectionalLight.lightBase.ambient * vec3(texture(u_Material.texture_diffuse1, v_TexCoords)) * u_GlobalAmbientStrength;
	
	//multiple Ambient Colour
	//ambientColour += u_DirectionalLight.lightBase.ambient * vec3(texture(u_Material.texture_diffuse2, v_TexCoords)) * u_GlobalAmbientStrength;
	//ambientColour += u_DirectionalLight.lightBase.ambient * vec3(texture(u_Material.texture_diffuse3, v_TexCoords)) * u_GlobalAmbientStrength;
	
	vec3 diffuseColour = CalDiffuseLightDir() * u_GlobalDiffuseStrength;
		
	vec3 specularColour = CalSpecularColour() * u_GlobalSpecularStrength;
	
	return ambientColour + diffuseColour + specularColour;
}


void main()
{

	vec3 dirLight_result = DirectionalLightCommulation();	
	
	vec3 emissionColour = vec3(0.0f);
	
	if(/*u_Material.emit*/false)
	{
		//vec3 emissionMask = step(vec3(1.0f), vec3(1.0f) - texture(u_Material.specularMap, v_TexCoords).rgb);
		//emissionColour = u_Material.emissionColour * vec3(texture(u_Material.emissionMap, v_TexCoords)) * u_Material.emissionMapStrength * emissionMask;
		emissionColour = vec3(texture(u_Material.texture_emissive1, v_TexCoords)) * u_Material.emissionIntensity;
		//if(texture(u_Material.specularMap, v_TexCoords).r == 0.0f)
			//emissionColour = u_Material.emissionColour * vec3(texture(u_Material.emissionMap, v_TexCoords)) * u_Material.emissionMapStrength;
	}	

	vec3 finalColour = dirLight_result + emissionColour;
	o_FragColour = vec4(finalColour, 1.0f);
}