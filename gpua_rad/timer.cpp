#include "timer.h"

CTimer::CTimer(const char * identifier) : m_szIdentifier(identifier)
{
	m_tpscStart = std::chrono::high_resolution_clock::now();
	m_bEnded = false;
}

CTimer::~CTimer()
{
	if (!m_bEnded)
	{
		std::chrono::duration<double> elapsed = std::chrono::high_resolution_clock::now() - m_tpscStart;
		pp::Info("[Performance Timer] %s has ended, elapsed time: %f seconds", m_szIdentifier, elapsed.count());
		m_bEnded = true;
	}
}

void CTimer::End()
{
	if (!m_bEnded)
	{
		std::chrono::duration<double> elapsed = std::chrono::high_resolution_clock::now() - m_tpscStart;
		pp::Info("[Performance Timer] %s has ended, elapsed time: %f seconds", m_szIdentifier, elapsed.count());
		m_bEnded = true;
	}
}
