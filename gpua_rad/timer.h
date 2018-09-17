#ifndef _GPUARAD_TIMER_H_
#define _GPUARAD_TIMER_H_

#include <chrono>

#include "prettyprint.h"

class CTimer
{
public:
	CTimer(const char* identifier);
	~CTimer();
	void End();
private:
	std::chrono::time_point<std::chrono::system_clock> m_tpscStart;
	const char* m_szIdentifier;
	bool m_bEnded;
};

#endif // _GPUARAD_TIMER_H_