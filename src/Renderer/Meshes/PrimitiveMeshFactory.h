#pragma once

#include <memory>
#include <vector>

class Mesh;
struct Vertex;
namespace CRRT
{
	class PrimitiveMeshFactory
	{
	public:
		PrimitiveMeshFactory(const PrimitiveMeshFactory&) = delete;

		static inline PrimitiveMeshFactory& Instance()
		{
			static PrimitiveMeshFactory instance;
			return instance;
		}


		std::shared_ptr<Mesh> CreateQuad();
		std::shared_ptr<Mesh> CreateSphere(unsigned int sector_count = 36, unsigned int span_count = 18);
		std::shared_ptr<Mesh> CreateCube();

		Mesh CreateACube();
		Mesh CreateASphere(unsigned int sector_count = 36, unsigned int span_count = 18);
	/*	template<class T>
		T CreateAQuad();*/
		Mesh CreateAQuad();

	private:
		PrimitiveMeshFactory() = default;


		//probably cache a ref pointer to created mesh 


		//Helper 
		void ConvertVerticesArrayToVector(float* data, unsigned int size, unsigned int offset, std::vector<Vertex>& vertices);
	};


}