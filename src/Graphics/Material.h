#pragma once
#pragma once
#include "Shader.h"

class Material
{
public:
	Material(Shader& shader);

	void Use();
private:
	std::shared_ptr<Shader> m_Shader;

	float m_SpecularIntensity;
	float m_Shininess;
};