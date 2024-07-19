#include "Material.h"
#include "Renderer.h"


Material::Material(const Shader& shader)
{
	m_Shader = std::make_shared<Shader>(shader);
	m_Metallic = 1.0f;
	m_Smoothness = 1.0f;
}

void Material::Use()
{
	if (!m_Shader)
		return;

	m_Shader->UseShader();
	GLCall(glUniform1f(m_Shader->GetUniformLocation("u_Material.metallic"), m_Metallic));
	GLCall(glUniform1f(m_Shader->GetUniformLocation("u_Material.smoothness"), m_Smoothness));
	//m_GlobalShaderProgram.SetUniformVec3("u_Material.colour", mat.colour);
}
