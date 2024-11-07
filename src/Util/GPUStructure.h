#pragma once



namespace CRRT
{



	struct EnvironmentData
	{
		bool useSkybox;
		float skyInfluencity;
		//???float
		float alignment;

		static long long int GetGPUSize()
		{
			return sizeof(int) + //uint use in gpu in place of bool 1 >> 4 creates missmatching size
				   sizeof(skyInfluencity) + 
				   sizeof(float) + //test reflection
				   sizeof(alignment);
		}
	};
}