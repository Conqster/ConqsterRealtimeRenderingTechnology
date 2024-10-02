#pragma once
#include "Mesh.h"
#include <vector>

class SphereMesh : public Mesh
{
private:

	//NAME CONVECTION HELP: https://help.autodesk.com/view/MAYAUL/2022/ENU/?guid=GUID-BE2DAF5E-2EF9-48B0-80B6-AB65533D6E9F
	unsigned int m_SectorCount = 36; // 16;
	unsigned int m_SpanCount = 18; // 8;

	std::vector<float> m_Vertices;
	std::vector<unsigned int> m_Indices;
public:
	SphereMesh();
	virtual void Create() override;
	void Create(unsigned int segment);

	int SectorCount;
	int SpanCount;

	inline unsigned int const GetVerticesCount() const { return m_Vertices.size(); }
	inline unsigned int const GetIndicesCount() const { return m_Indices.size(); }

	void Update();
};