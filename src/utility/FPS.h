#pragma once

#include "SmartPtr.h"

namespace Lazy
{

    class Fps : public IBase
    {
    public:
        Fps(void);

        void init( void );

        void update(void);

        //获得当前时间，单位秒
        float getCurTime(void) const;

    public:

        float getStartTime(void) const { return m_startTime; }

        int getTotalFrame(void) const { return m_totalFrame; }

        float getFps(void) const { return m_fps; }

        float getElapse(void) const { return m_elapse; }

    private:
        float m_startTime;		    //启动时间

        int m_totalFrame;           //从游戏启动到现在经过的帧数
        int m_lastUpdateFpsFrame;   //上次更新fps时的帧数
        float m_lastUpdateFpsTime;	//上次更新fps时的时间

        float m_fps;			    //帧刷新率

        float m_lastUpdateTime;
        float m_elapse;
    };


}//namespace Lazy