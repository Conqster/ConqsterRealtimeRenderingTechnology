#pragma once
#include "Texture.h"
#include "glm/glm.hpp"
#include <memory>

///A Mesh does not know about materials & Vices versa
class Shader;
struct Material
{
	Material() = default;

	const char* name = "default";
	int id = 0; //<-------- temp id
	//later add a shader ref
	std::shared_ptr<Shader> shader = nullptr;

	glm::vec3 baseColour = glm::vec3(1.0f); //base coour contributes to base map (* multply as a mask)

	std::shared_ptr<Texture> baseMap = nullptr;
	bool useNormal = true;
	std::shared_ptr<Texture> normalMap = nullptr;
	bool useParallax = false;
	std::shared_ptr<Texture> parallaxMap = nullptr;
	float heightScale = 0.0f; //parallax scale
	int shinness = 64;
};