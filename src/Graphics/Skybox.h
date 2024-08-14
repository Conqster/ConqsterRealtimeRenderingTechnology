#pragma once

#include "TextureCube.h";
#include "Meshes/CubeMesh.h"
#include "Shader.h"




class Skybox
{
public:
	Skybox();
	Skybox(const std::vector<std::string> faces_image_path);
	
	void Create(const std::vector<std::string> faces_image_path);
	
	//ned to fixed the shader later as void Draw() const; 
	//Doesnt work because i am modifing the shader indirect if i call setUniforms
	//as i would cache the uniform is it exist but not been cached
	void Draw(class Camera& camera, class Window& window);
	//void ActivateMap()

	inline Shader* const GetShader() { return &m_Shader; }
	inline TextureCube* const GetTextureMap() { return &m_TextureMap; }
	inline Mesh* const GetMesh() { return &m_Mesh; }

	void Destroy();
private:
	std::vector<std::string> m_FacePath;

	TextureCube m_TextureMap;
	CubeMesh m_Mesh;
	Shader m_Shader;
};