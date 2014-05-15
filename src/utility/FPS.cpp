
#include "stdafx.h"
#include "FPS.h"

#include <MMSystem.h>

#pragma comment(lib, "winmm.lib")

namespace Lazy
{

Fps::Fps(void)
    : m_fps(0.0f)
    , m_elapse(0.0f)
    , m_totalFrame(0)
    , m_lastUpdateFpsFrame(0)
{
   m_startTime = getCurTime();
   m_lastUpdateFpsTime = m_startTime;
   m_lastUpdateTime = m_startTime;
}

void Fps::init( void )
{
	
}

void Fps::update( void )
{
    float curTime = getCurTime();

    m_elapse = curTime - m_lastUpdateTime;
    m_lastUpdateTime = curTime;

    ++m_totalFrame;
    float dtime = curTime - m_lastUpdateFpsTime;
	if(dtime > 1.0f)//每1秒更新一次
	{
		m_fps = (m_totalFrame - m_lastUpdateFpsFrame) / dtime;
        m_lastUpdateFpsTime = curTime;
        m_lastUpdateFpsFrame = m_totalFrame;
	}
}

float Fps::getCurTime(void) const
{
	return timeGetTime()* 0.001f;
}



}//namespace Lazy