#pragma once

#include <vector>
#include <cstdlib>
#include <ctime>

struct MathsHelper
{

	static inline double Rad2Deg(double degrees)
	{
		return degrees * 57.295779513082320876798154814105;
	}

	static inline double Deg2Rad(double radians)
	{
		return radians * 0.01745329251994329576923690768489;
	}

	static constexpr double PI = 3.141592653589793238462643;

	
	////////////////////////////////////
	///////Matrix///////////////////////
	//|00 10 20 30|//////|Rx Rx Rx Px|//
	//|01 11 21 31|//>>//|Ry Ry Ry Py|//
	//|02 12 22 32|//>>//|Rz Rz Rz Pz|//
	//|03 13 23 33|//////| 0  0  0  1|//
	////////////////////////////////////



	static inline void DecomposeTransform(const glm::mat4& transform, glm::vec3& translate, glm::vec3& euler, glm::vec3& scale)
	{
		glm::mat4 Mt = transform;
		translate = glm::vec3(Mt[3]);

		if (std::abs(Mt[3][3] - 1.0f) > 1e-6f)
			return;

		for (int i = 0; i < 4; ++i)
			for (int j = 0; j < 4; ++j)
				if (std::isnan(Mt[i][j]))
					return;

		//Remove translation for scale & rot extraction 
		Mt[3] = glm::vec4(0.0f, 0.0f, 0.0f, Mt[3].w); 

		scale.x = glm::length(glm::vec3(Mt[0][0], Mt[1][0], Mt[2][0]));
		scale.y = glm::length(glm::vec3(Mt[0][1], Mt[1][1], Mt[2][1]));
		scale.z = glm::length(glm::vec3(Mt[0][2], Mt[1][2], Mt[2][2]));

		if (glm::determinant(Mt) < 0)
			scale.x = -scale.x;

		glm::mat3 Mr;
		Mr[0] = glm::vec3(Mt[0][0], Mt[1][0], Mt[2][0]) / scale.x;
		Mr[1] = glm::vec3(Mt[0][1], Mt[1][1], Mt[2][1]) / scale.y;
		Mr[2] = glm::vec3(Mt[0][2], Mt[1][2], Mt[2][2]) / scale.z;

		euler.x = atan2(Mr[2][1], Mr[2][2]);
		euler.y = atan2(-Mr[2][0], glm::sqrt(Mr[2][1] * Mr[2][1] + Mr[2][2] * Mr[2][2]));
		euler.z = atan2(Mr[1][0], Mr[0][0]);

		euler = glm::degrees(euler);
	}



	static inline std::vector<glm::vec3> CubeLocalVertices()
	{
		return	
		{
			//front
			{-0.5f, 0.5f, 0.5f}, {-0.5f, -0.5f, 0.5f}, {0.5f, -0.5f, 0.5f}, {0.5f, 0.5f, 0.5f},
			//back
			{-0.5f, 0.5f, -0.5f}, {-0.5f, -0.5f, -0.5f}, {0.5f, -0.5f, -0.5f}, {0.5f, 0.5f, -0.5f},
		};
	} 


	//quick random value between two value, min & max
	static inline float RandomFloat(float min, float max)
	{
		return min + static_cast<float>(rand()) / (static_cast<float>(RAND_MAX / (max - min)));
	}


	//quick random point but not fully uniform 
	static inline glm::vec3 RandomPointInSphere(float radius)
	{
		if (radius <= 0.0f)
			return glm::vec3(0.0f);

		while (true)
		{
			float x = RandomFloat(-radius, radius);
			float y = RandomFloat(-radius, radius);
			float z = RandomFloat(-radius, radius);

			printf("Random point in sphere x: %f, y: %f, z: %f\n", x, y, z);

			if (x * x + y * y + z * z <= radius * radius)
				return glm::vec3(x, y, z);
		}
	}


	static inline glm::vec3 RandomColour()
	{
		float r = RandomFloat(0, 1);
		float g = RandomFloat(0, 1);
		float b = RandomFloat(0, 1);

		return glm::vec3(r, g, b);
	}
};
