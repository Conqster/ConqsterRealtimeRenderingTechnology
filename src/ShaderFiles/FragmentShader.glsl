#version 400

//v from vertex
in vec4 v_colour;
in vec2 v_texCoord;
in vec3 v_testNormal;
in vec3 v_fragPos;

out vec4 outColour;

uniform vec4 u_TestColour;
uniform sampler2D u_Texture;
uniform float u_DeltaColour;

uniform bool u_DebugMode;

uniform vec4 u_LightColour;
uniform vec3 u_LightPos;
uniform float u_AmbientStrength;
uniform float u_DiffuseIntensity;

struct Light
{
	bool isEnabled;
	vec3 colour;
	float ambientIntensity;
	float diffuseIntensity;
};

struct PointLight
{
	Light base;
	vec3 position;

	//float constantAttenuation;
	//float linearAttenuation;
	//float quadraticAttenuation;
	//TO-DO: Stored all attenuation contiguously in memory for easy access
	//		Probably change to a vec3 later
	vec3 attenuation;
};

struct SpotLight
{
	PointLight pointLight;
	vec3 direction;

	float falloff;
};

struct DirectionalLight
{
	Light base;
	vec3 direction;
};

struct Material
{
	float metallic;
	float smoothness;

	vec3 colour;
};

uniform int u_PointLightCount;
const int MAX_POINT_LIGHT = 5;
uniform PointLight u_PointLights[MAX_POINT_LIGHT];

uniform int u_SpotLightCount;
const int MAX_SPOT_LIGHT = 5;
uniform SpotLight u_SpotLights[MAX_SPOT_LIGHT];

uniform int u_DirectionalLightCount; 
const int MAX_DIRECTIONAL_LIGHT = 2;
uniform DirectionalLight u_DirectionalLights[MAX_DIRECTIONAL_LIGHT];

uniform vec3 u_ViewPos;
uniform Material u_Material;
uniform bool u_UseNew = true;
vec3 testColour;


//////////////////CALCULATE SPECULAR REFLECTION////////////////////////////////////////
vec3 CalculateSpecularReflection(vec3 surfaceNormal, vec3 lightDir, vec3 lightColour)
{
	//max metallic == 200 && max smoothness == 50 but reversed
	float specularPower = (u_Material.metallic + 0.01f) * 300;
	float specularFactor =(u_Material.smoothness * 8.0f * 2.0f) ;//* 50;
	float pi = 3.14159265f;
	float energyConservation = (specularFactor * specularPower) / (specularPower * pi);

	vec3 viewDir = normalize(u_ViewPos - v_fragPos);
	vec3 halfWayDir = normalize(lightDir + viewDir);
	float highlight = pow(max(dot(surfaceNormal, halfWayDir), 0.0f), specularPower);

	vec3 specularColour = mix(lightColour, testColour , u_Material.metallic);
	//specularColour = lightColour;
	
	//NEW NEW 
	if(u_UseNew)
	{
		//MIGHT CHANGE THIS: texture(u_Texture, v_texCoord).rgb to specular object material map
		//MIGHT CHANGE THIS: (lightColour * light.specular) to light spcenular colour
		return (highlight * (lightColour /* light.specular*/) * vec3(texture(u_Texture, v_texCoord)));
	}
	
	return energyConservation * (highlight * specularColour);
}


//CALCULATE DIFFUSE
vec3 LightDiffuse(Light light, vec3 lightDir)
{
	vec3 norm = normalize(v_testNormal);
	float diffuseFactor = max(dot(norm, lightDir), 0.0f);
	vec3 diffuseColour = vec3(light.colour) * diffuseFactor * light.diffuseIntensity;

	vec3 specular = vec3(0.0f);
	vec3 colour = vec3(0.0f);
	if(diffuseFactor > 0.0f)//
	{
		//TO-DO: test reflection 
		vec3 surfaceNormal = norm;
		specular = CalculateSpecularReflection(surfaceNormal, lightDir, light.colour);
	}

	//NEW NEW 
	if(u_UseNew)
	{
		//MIGHT CHANGE THIS: texture(u_Texture, v_texCoord).rgb to diffuse object material map
		diffuseColour = light.colour * diffuseFactor * light.diffuseIntensity * texture(u_Texture, v_texCoord).rgb;
	}
	return diffuseColour + specular;
}


vec3 CalculatePointLights()
{
	vec3 finalColour = vec3(0.0f);
	
	for(int i = 0; i < u_PointLightCount; i++)	
	{
		if(u_PointLights[i].base.isEnabled)
			continue;
		
		vec3 ambient = u_PointLights[i].base.ambientIntensity * u_PointLights[i].base.colour;
		
		//NEW NEW 
		if(u_UseNew)
		{
			//MIGHT CHANGE THIS: texture(u_Texture, v_texCoord).rgb to diffuse object material map
			ambient = u_PointLights[i].base.ambientIntensity * u_PointLights[i].base.colour * texture(u_Texture, v_texCoord).rgb;
		}
		vec3 lightDir = normalize(u_PointLights[i].position - v_fragPos);
		vec3 diffuse = LightDiffuse(u_PointLights[i].base, lightDir);
		vec3 resultColour = ambient + diffuse;
		//vec3 resultColour = ambient * diffuse;

		vec3 currentFragDir = u_PointLights[i].position - v_fragPos;
		float distance = length(currentFragDir);

		float totalAttenuation = u_PointLights[i].attenuation.x +
								(u_PointLights[i].attenuation.y * distance) +
								(u_PointLights[i].attenuation.z * distance * distance);

		finalColour += (resultColour/totalAttenuation);
	}

	return finalColour;
}




vec3 CalculateSpotLight()
{
	vec3 finalColour = vec3(0.0f);

	for(int i = 0; i < u_SpotLightCount; i++)	
	{
		if(u_SpotLights[i].pointLight.base.isEnabled)
			continue;
		
		vec3 ambient = u_SpotLights[i].pointLight.base.ambientIntensity * u_SpotLights[i].pointLight.base.colour;
		
		//NEW NEW 
		if(u_UseNew)
		{
			//MIGHT CHANGE THIS: texture(u_Texture, v_texCoord).rgb to diffuse object material map
			ambient = u_SpotLights[i].pointLight.base.ambientIntensity * u_SpotLights[i].pointLight.base.colour * texture(u_Texture, v_texCoord).rgb;
		}
		vec3 lightDir = normalize(u_SpotLights[i].pointLight.position - v_fragPos);
		vec3 diffuse = LightDiffuse(u_SpotLights[i].pointLight.base, lightDir);
		//vec3 diffuse2 = LightDiffuse(u_SpotLights[i].pointLight.base.diffuseIntensity, u_SpotLights[i].pointLight.position, u_SpotLights[i].pointLight.base.colour);
		//vec3 resultAmbientDiffColour = ambient + diffuse;
		vec3 resultAmbientDiffColour = ambient * diffuse;
		
		//NEW NEW
		if(u_UseNew)
			resultAmbientDiffColour = ambient + diffuse;
		

		vec3 currentFragDir = u_SpotLights[i].pointLight.position - v_fragPos;
		float distanceLength = length(currentFragDir);    //length == distance
		//unit length direction pointing from point(frag) in space
		vec3 lengthDirection = currentFragDir / distanceLength;

		//Direction based falloff  ## max{-R.L, 0}^p ##
		//where R = spot light direction, L = Length direction(point(frag),lightpos), p = spot Light falloff
		float directionFalloff = pow(max(dot(-u_SpotLights[i].direction, lengthDirection), 0.0f), u_SpotLights[i].falloff);


		float totalAttenuation = u_SpotLights[i].pointLight.attenuation.x +
								(u_SpotLights[i].pointLight.attenuation.y * distanceLength) + 
								(u_SpotLights[i].pointLight.attenuation.z * distanceLength * distanceLength);

		finalColour += (directionFalloff/totalAttenuation) * resultAmbientDiffColour;
	}

	return finalColour;
}

vec3 CalculateDirectionalLight()
{
	vec3 resultColour = vec3(0.0f);
	
	for(int i = 0; i < u_DirectionalLightCount; i++)	
	{
		if(u_DirectionalLights[i].base.isEnabled)
			continue;
		
		vec3 ambientColour = u_DirectionalLights[i].base.ambientIntensity * u_DirectionalLights[i].base.colour;
		
		//NEW NEW 
		if(u_UseNew)
		{
			//MIGHT CHANGE THIS: texture(u_Texture, v_texCoord).rgb to diffuse object material map
			ambientColour = u_DirectionalLights[i].base.ambientIntensity * u_DirectionalLights[i].base.colour * texture(u_Texture, v_texCoord).rgb;
		}
		vec3 diffuseColour = LightDiffuse(u_DirectionalLights[i].base, u_DirectionalLights[i].direction);
		//resultColour += ambientColour + diffuseColour;
		
		if(!u_UseNew)
			resultColour += (ambientColour * diffuseColour);
		
		//NEW NEW
		if(u_UseNew)
		{
			resultColour += (ambientColour * diffuseColour);
		}
	}
	return resultColour;
}

void main()
{ 
	vec4 texColour = texture(u_Texture, v_texCoord);
	testColour = vec3(texColour.x, texColour.y, texColour.z); /// later might get the pixel colour instead6-
	//Testing colour 
	vec3 finalLightColour = vec3(0.0f);
	finalLightColour += CalculateDirectionalLight();
	finalLightColour += CalculatePointLights();
	finalLightColour += CalculateSpotLight();

	//vec4 texColour = texture(u_Texture, v_texCoord);
	outColour = texColour * (v_colour + u_DeltaColour) + u_TestColour;
	//outColour = vec4(ambient, 1.0f) * texColour;
	outColour = vec4(finalLightColour, 1.0f) * texColour;
	
	//NEW NEW 
	if(u_UseNew)
		outColour = vec4(finalLightColour, 1.0f);

	if(u_DebugMode)
	{
		outColour = v_colour;
		//outColour = vec4(0.0f, 1.0f, 0.0f, 1.0f);   //green colour 
	}
}