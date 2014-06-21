#pragma once

#include "RenderObj.h"
#include "..\third_part\audiere\audiere.h"
#include "LRCLoader.h"

#ifndef RT_VOID
#   define RT_VOID
#endif

#define SafeCheck(p, r) if(!p)return r

namespace Lazy
{

    class LZDLL_API MusicPlayer : public IBase , public IUpdate
    {
    public:
        MusicPlayer(void);
        ~MusicPlayer(void);

        bool loadMusic(LPCSTR szName);
        //显示
        //void render(HDC hDC);
        virtual void update(float elapse);

        //播放
        void play(void) ;
        //暂停
        void pause(void);
        //停止
        void stop(void) ;

        //是否正在播放
        bool isPlaying(void);

        //重置。将播放位置置于开头。
        void reset(void);

        //设置重复播放
        void setRepeat(bool repeat);
        bool getRepeat(void);

        //设置音量
        void setVolume(float volume);
        float getVolume(void);

        //设置音效平衡
        void setPan(float pan);
        float getPan(void);

        void setPitchShift(float shift);
        float getPitchShift(void);

        bool isSeekable(void);
        //获得音频长度。单位为帧。
        int getLength(void);
        //设置播放位置。单位为帧。
        void setPosition(int position);
        int getPosition(void);

        void setFade(bool fade) { m_bFade = fade; }

        void setFadeTime(float time) { m_nFadeTime = time; }

    private:
        RefPtr<CLRCLoader>      m_pLrcLoader;
        bool                    m_bLRCEnable;
        audiere::RefPtr<audiere::AudioDevice> m_pDevice;
        audiere::RefPtr<audiere::OutputStream> m_pStream;
        std::string             m_strText;

        int     m_nCommand;     //控制命令。1-播放，2-暂停，3-停止。
        bool    m_bFade;        //是否淡入淡出
        float   m_nFadeTime;    //淡化时间
        float   m_nElapseTime;  //已经淡化时间
        float   m_fOldVolum;    //音量
    };


} // end namespace Lazy
