#pragma once
#include <chrono>
#include <thread>
#include <iostream>

class GameTime
{
public: 

	GameTime()
	{
		m_DeltaTime = 0.0f;
		m_TimePreviousFrame = std::chrono::high_resolution_clock::now();
		std::cout << "Game Time init.................................\n";
	}

	void Init(double targetFPS)
	{
		m_TargetMsPerFrame = 1.0 / targetFPS;
		m_TimePreviousFrame = std::chrono::high_resolution_clock::now();
	}

	//template<typename T>
	//T DeltaTime() const;

	//template<>
	//double DeltaTime() const
	//{
	//	return m_DeltaTime;
	//}

	//template<>
	//float DeltaTime() const
	//{
	//	return (float)m_DeltaTime;
	//}

	float DeltaTime() const
	{
		return  (float)m_DeltaTime;
	}



	double DeltaTimeInMillesec()
	{
		return m_DeltaTime * 1000.0f;
	}

	int FPS()
	{
		//return static_cast<int>(std::round(1.0f / m_DeltaTime));
		return (int)(1.0 / m_DeltaTime);
	}




	void Update()
	{
		m_Current = std::chrono::high_resolution_clock::now();


		std::chrono::duration<double> elapsed = m_Current - m_TimePreviousFrame;
		//elapsed = m_Current - m_TimePreviousFrame;
		//elapsed = std::chrono::duration<double>(m_Current - m_TimePreviousFrame).count();

		//std::chrono::duration_cast<std::chrono::nanoseconds>

		//std::cout << "Current deltaTime: " << elaspsed.count() << "\n";
		//double currentDt = elaspsed.c
		//double timeToWait = m_TargetMsPerFrame - elapsed.count();


		if (false)
		{
			double timeToWait = m_TargetMsPerFrame - (std::chrono::duration_cast<std::chrono::nanoseconds>(std::chrono::high_resolution_clock::now() - m_TimePreviousFrame).count()) * 0.001f * 0.001f * 0.001f;
			//std::cout << "Time to wait for " << timeToWait << std::endl;
			if (timeToWait > 0)
				std::this_thread::sleep_for(std::chrono::duration<double>(timeToWait));
		}


		m_Current = std::chrono::high_resolution_clock::now();
		//m_DeltaTime = elaspsed.count();
		//m_DeltaTime = (m_Current - m_TimePreviousFrame).count();
		m_DeltaTime = std::chrono::duration_cast<std::chrono::duration<double>>(m_Current - m_TimePreviousFrame).count();

		m_TimePreviousFrame = m_Current;
	}


private: 
	std::chrono::time_point<std::chrono::high_resolution_clock> m_Current;
	std::chrono::time_point<std::chrono::high_resolution_clock> m_TimePreviousFrame;

	double m_DeltaTime;
	//double m_TargetMsPerFrame = 0.016667;
	double m_TargetMsPerFrame = 0;
	
	const float millisec2Sec = 0.001f;
	const float sec2Millisec = 1000.0f;
};


struct TimeTaken
{
	std::chrono::time_point<std::chrono::steady_clock> start;
	std::chrono::time_point<std::chrono::steady_clock> end;

	std::chrono::duration<float> duration;

	const char* m_info;

	TimeTaken(const char* info) : m_info(info), duration(0.0f)
	{
		start = std::chrono::high_resolution_clock::now();
	}

	~TimeTaken()
	{
		end = std::chrono::high_resolution_clock::now();
		duration = end - start;

		float duration_ms = duration.count() * 1000.0f;
		std::cout << m_info << " - time took: " << duration_ms << "ms \n";
	}
};


