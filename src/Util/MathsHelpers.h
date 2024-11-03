#pragma once

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
};
