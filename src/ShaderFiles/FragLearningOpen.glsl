#version 400

in vec4 v_Colour;
in vec3 v_Normals;
in vec3 v_FragPos;
in vec2 v_TexCoord;

out vec4 o_Colour;

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

struct Light
{
	vec3 position;
	
	vec3 ambient;
	vec3 diffuse;
	vec3 specular;
};


uniform sampler2D u_Texture;
uniform Material u_Material;
uniform Light u_light;
uniform vec3 u_LightColour;
uniform bool u_IsLight;

uniform float u_GlobalAmbientStrength;
uniform float u_GlobalDiffuseStrength;
uniform float u_GlobalSpecularStrength;

uniform vec3 u_ViewPos;


vec3 CalDiffuseLightDir()
{
	vec3 resultColour = vec3(0.0f);
	vec3 norm = normalize(v_Normals);
	vec3 lightDir = normalize(u_light.position - v_FragPos);
	
	float diff = max(dot(norm, lightDir), 0.0f);
	//resultColour = (diff * u_Material.diffuse)* u_LightColour;
	//since u_Material diffuse is no more a vec3 now a sampler 2d texColour
	resultColour = u_light.diffuse * diff * vec3(texture(u_Material.diffuseMap, v_TexCoord));
	return resultColour;
}


vec3 CalSpecularColour()
{
	vec3 lightDir = normalize(u_light.position - v_FragPos);
	vec3 viewDir = normalize(u_ViewPos - v_FragPos);
	vec3 norm = normalize(v_Normals);
	vec3 reflectDir = reflect(-lightDir, norm);
	
	float spec = pow(max(dot(viewDir, reflectDir), 0.0), u_Material.shininess);
	
	vec3 specular = vec3(1.0f);
	if(u_Material.invertSpecularMap)
		specular = u_light.specular * spec *  (vec3(1.0f) - vec3(texture(u_Material.specularMap, v_TexCoord)));//u_LightColour * (spec * u_Material.specular);
	else
		specular = u_light.specular * spec *  (vec3(texture(u_Material.specularMap, v_TexCoord)));//u_LightColour * (spec * u_Material.specular);
	//vec3 specular = u_light.specular * spec *  ((vec3(texture(u_Material.specularMap, v_TexCoord) * -1) + vec3(1.0f))  );
	return specular;
}


void main()
{
	//o_Colour = v_Colour * vec4(u_Material.diffuse, 1.0f);
	//o_Colour = vec4((u_LightColour * u_ToyColour), 1.0f);  //ToyColour old object colour
	vec4 texColour = texture(u_Texture, v_TexCoord);
	

	if(u_IsLight)
	{
		o_Colour = vec4(u_LightColour, 1.0f);
		//influence light object with light ambient, diffuse and specular colour 
		//o_Colour = vec4(u_light.ambient * u_light.diffuse * u_light.specular, 1.0f);
	}
	else
	{
			//ambient
		//vec3 ambient = u_LightColour * u_Material.ambient * u_GlobalAmbientStrength; //u_AmbientStrength
		//since u_Material diffuse is no more a vec3 now a sampler 2d texColour
		vec3 ambientColour = u_light.ambient * vec3(texture(u_Material.diffuseMap, v_TexCoord)) * u_GlobalAmbientStrength;
		vec3 diffuseColour = CalDiffuseLightDir() * u_GlobalDiffuseStrength;
		vec3 specularColour = CalSpecularColour() * u_GlobalSpecularStrength;
		//vec3 finalColour = (ambientColour * u_light.ambient + diffuseColour * u_light.diffuse + specularColour * u_light.specular) ;//* u_ToyColour;
		
		vec3 emissionColour = vec3(0.0f);
		if(u_Material.emit)
		{
			vec3 emissionMask = step(vec3(1.0f), vec3(1.0f) - texture(u_Material.specularMap, v_TexCoord).rgb);
			emissionColour = u_Material.emissionColour * vec3(texture(u_Material.emissionMap, v_TexCoord)) * u_Material.emissionMapStrength * emissionMask;
			//if(texture(u_Material.specularMap, v_TexCoord).r == 0.0f)
				//emissionColour = u_Material.emissionColour * vec3(texture(u_Material.emissionMap, v_TexCoord)) * u_Material.emissionMapStrength;
		}			

		vec3 finalColour = ambientColour + diffuseColour + specularColour + emissionColour;
		o_Colour = vec4(finalColour, 1.0f);// * texColour;
	}
}