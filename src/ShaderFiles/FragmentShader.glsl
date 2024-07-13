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

uniform int u_PointLightCount;
const int MAX_POINT_LIGHT = 5;
uniform PointLight u_PointLights[MAX_POINT_LIGHT];

uniform int u_SpotLightCount;
const int MAX_SPOT_LIGHT = 5;
uniform SpotLight u_SpotLights[MAX_SPOT_LIGHT];

uniform int u_DirectionalLightCount; 
const int MAX_DIRECTIONAL_LIGHT = 2;
uniform DirectionalLight u_DirectionalLights[MAX_DIRECTIONAL_LIGHT];

//CALCULATE DIFFUSE
vec3 LightDiffuse(float intensity, vec3 position, vec3 lightColour)
{
	vec3 norm = normalize(v_testNormal);
	vec3 lightDir = normalize(position - v_fragPos);
	float diffuseFactor = max(dot(norm, lightDir), 0.0f);
	vec3 diffuseColour = vec3(lightColour) * diffuseFactor * intensity;
	return diffuseColour;
}


vec3 CalculatePointLights()
{
	vec3 finalColour = vec3(0.0f);
	
	for(int i = 0; i < u_PointLightCount; i++)	
	{
		vec3 ambient = u_PointLights[i].base.ambientIntensity * u_PointLights[i].base.colour;
		vec3 diffuse = LightDiffuse(u_PointLights[i].base.diffuseIntensity, u_PointLights[i].position, u_PointLights[i].base.colour);
		vec3 resultColour = ambient + diffuse;

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
		vec3 ambient = u_SpotLights[i].pointLight.base.ambientIntensity * u_SpotLights[i].pointLight.base.colour;
		//vec3 diffuse = LightDiffuse(u_SpotLights[i].pointLight.base.diffuseIntensity, u_Spotlights[i].pointLight.position, u_SpotLights[i].pointLight.base.colour);
		vec3 diffuse2 = LightDiffuse(u_SpotLights[i].pointLight.base.diffuseIntensity, u_SpotLights[i].pointLight.position, u_SpotLights[i].pointLight.base.colour);
		vec3 resultAmbientDiffColour = ambient + diffuse2;
		

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
		vec3 ambientColour = u_DirectionalLights[i].base.ambientIntensity * u_DirectionalLights[i].base.colour;

		vec3 norm = normalize(v_testNormal);
		float diffuseFactor = max(dot(norm, u_DirectionalLights[i].direction), 0.0f);
		vec3 diffuseColour = vec3(u_DirectionalLights[i].base.colour) * diffuseFactor * u_DirectionalLights[i].base.diffuseIntensity;
		resultColour += ambientColour + diffuseColour;
	}
	return resultColour;
}

void main()
{ 

	vec3 finalLightColour = vec3(0.0f);
	finalLightColour += CalculateDirectionalLight();
	finalLightColour += CalculatePointLights();
	finalLightColour += CalculateSpotLight();

	vec4 texColour = texture(u_Texture, v_texCoord);
	outColour = texColour * (v_colour + u_DeltaColour) + u_TestColour;
	//outColour = vec4(ambient, 1.0f) * texColour;
	outColour = vec4(finalLightColour, 1.0f) * texColour;

	if(u_DebugMode)
	{
		outColour = v_colour;
		//outColour = vec4(0.0f, 1.0f, 0.0f, 1.0f);   //green colour 
	}
}