#pragma once
#include "Texture.h"
#include "glm/glm.hpp"
#include <memory>

struct Material
{
	Material() = default;

	const char* name = "default";
	//later add a shader ref

	glm::vec3 baseColour = glm::vec3(1.0f); //base coour contributes to base map (* multply as a mask)

	std::shared_ptr<Texture> baseMap = nullptr;
	std::shared_ptr<Texture> normalMap = nullptr;
	bool useParallax = false;
	std::shared_ptr<Texture> parallaxMap = nullptr;
	float heightScale = 0.0f; //parallax scale
	int shinness = 64;
};