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
	vec3 diffuse;
	vec3 ambient;
	vec3 specular;
};


//------------------------ Parameters ----------------------------/
in VS_OUT
{
	vec3 fragPos;
	vec3 viewPos;
	vec3 normal;
}fs_in;

uniform Material u_Material;
uniform bool u_OnlyBaseColour;

void main()
{
	//cam view direction
	vec3 V = normalize(fs_in.viewPos - fs_in.fragPos);
	
	vec3 N = normalize(fs_in.normal);
	
	vec3 out_ambient = u_DirectionalLight.ambient * u_Material.ambient;
	
	//diffuse component
	vec3 Ld = normalize(u_DirectionalLight.direction);//light direction
	//Lambert cosine law
	float factor = max(dot(N, Ld), 0.0f);
	vec3 out_diffuse = u_DirectionalLight.diffuse * u_Material.diffuse * factor;
	
	//specular component (Blinn-Phong)
	//halfway 
	vec3 H = normalize(Ld + V);
	float specularity = pow(max(dot(N, H), 0.0f), u_Material.shinness);
	vec3 out_specular = u_DirectionalLight.specular * u_Material.specular * specularity;
	
	vec3 final_light_contributation = out_ambient + out_diffuse + out_specular;
	FragColour = vec4(final_light_contributation, 1.0f);
	
	if(u_OnlyBaseColour)
		FragColour = vec4(u_Material.ambient + u_Material.diffuse, 1.0f);
}
