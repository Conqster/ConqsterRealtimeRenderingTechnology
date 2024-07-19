#pragma once
#pragma once
#include "Shader.h"

class Material
{
public:
	Material(const Shader& shader);

	inline float* const Ptr_Metallic() { return &m_Metallic; }
	inline float* const Ptr_Smoothness() { return &m_Smoothness; }

	//void SwapUseShader(Shader* new_shader);

	void Use();
private:
	std::shared_ptr<Shader> m_Shader;

	float m_Metallic;
	float m_Smoothness;

	int m_RefCount;
};