#pragma once
#include "Texture.h"
#include "glm/glm.hpp"
#include <memory>


namespace CRRT_Mat
{

	//Following unity material setup 
	enum class RenderingMode
	{
		Opaque,
		Transparent,
		Count,
	};

	inline const char** GetAllRenderingModesAsName()
	{
		static const char* rendering_mode[] = { "Opaque", "Transparent" };
		return rendering_mode;
	}

	//inline unsigned int TextureCount() { return 4; }
}



///A Mesh does not know about materials & Vices versa
class Shader;
struct Material
{
	Material() = default;

	CRRT_Mat::RenderingMode renderMode = CRRT_Mat::RenderingMode::Opaque;

	const char* name = "default";
	int id = 0; //<-------- temp id
	//later add a shader ref
	std::shared_ptr<Shader> shader = nullptr;

	glm::vec4 baseColour = glm::vec4(1.0f); //base coour contributes to base map (* multply as a mask)

	std::shared_ptr<Texture> baseMap = nullptr; //0
	bool useNormal = true;
	std::shared_ptr<Texture> normalMap = nullptr; //1
	bool useParallax = false;
	std::shared_ptr<Texture> parallaxMap = nullptr; //2
	std::shared_ptr<Texture> specularMap = nullptr; //3
	float heightScale = 0.0f; //parallax scale
	int shinness = 64;

	static const unsigned int MAX_MAP_COUNT = 4;
};




