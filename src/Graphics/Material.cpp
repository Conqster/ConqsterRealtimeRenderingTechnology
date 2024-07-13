#include "Material.h"
#include "Renderer.h"


Material::Material(Shader& shader)
{
}

void Material::Use()
{
	if (!m_Shader)
		return;

	m_Shader->UseShader();
	GLCall(glUniform1f(m_Shader->GetUniformLocation(""), m_SpecularIntensity));
	GLCall(glUniform1f(m_Shader->GetUniformLocation(""), m_Shininess));
}
