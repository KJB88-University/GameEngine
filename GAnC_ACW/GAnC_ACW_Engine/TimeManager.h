#pragma once
#include <chrono>
#include "Manager.h"

class TimeManager
	: public Manager
{
public:
	TimeManager();
	~TimeManager();

	void Update();

	static float GetDeltaTime();

private:

	std::chrono::steady_clock timer;

	std::chrono::steady_clock::time_point m_currentTime;
	std::chrono::steady_clock::time_point m_previousTime;
	static float m_deltaTime;

	TimeManager(const TimeManager&) = delete;
	TimeManager& operator=(const TimeManager&) = delete;
};