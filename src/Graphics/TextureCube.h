#pragma once

#include <string>
#include <vector>


//struct TextureFaceFile
//{
//	std::string rightFace;
//	std::string leftFace;
//	std::string topFace;
//	std::string bottomFace;
//	std::string backFace;
//	std::string frontFace;
//
//	std::vector<std::string> faces;
//
//	std::string Right() { return faces[0]; }
//};



/// <summary>
/// TO-DO: Probaly later have something called a shared texture 
///		   It might be similar to batch rendering i think
///		   so similar texture wouldnt be duplicated in memory
/// </summary>
class TextureCube
{
private:
	unsigned int m_Id;
	std::vector<std::string> m_FaceTextureFiles;
	unsigned char* m_LocalBuffer;
	int m_Width, m_Height, m_BitDepth;

	int m_nrChannels;

	int m_RefCount;

public:
	TextureCube();
	TextureCube(const std::vector<std::string>& fileLoc);
	~TextureCube();

	void RegisterUse();
	void UnRegisterUse();
	inline int const RefCount() const { return m_RefCount; }

	bool LoadTexture(const std::vector<std::string>& fileLoc);
	void Activate(uint16_t slot = 0) const;
	void DisActivate() const;
	void Clear();

	inline int GetWidth() const { return m_Width; }
	inline int GetHeight() const { return m_Height; }
};
