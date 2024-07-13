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
};
