#pragma once

#include "renderres.h"

namespace Lazy
{
    typedef IDirectSoundBuffer8*  LPDSBuffer;

//////////////////////////////////////////////////////////////////////////
    class LZDLL_API Sound : public IBase
    {
    public:
        Sound(std::string name, LPDSBuffer buffer);

        ~Sound(void);

        /*播放。*/
        bool play();

        /** 暂停*/
        bool pause();

        /*停止播放。*/
        bool stop();

        /*是否正在播放。*/
        bool isPlaying();

        /*设置频率。*/
        bool setFrequncy(long fre);

        /*设置平衡。*/
        bool setPan(long lpan);

        /*设置音量。*/
        bool setVolum(long volume);

        std::string getName(void) { return m_name; }
    protected:
        std::string     m_name;
        LPDSBuffer      m_pBuffer;
    };

    typedef RefPtr<Sound> SoundPtr;

//////////////////////////////////////////////////////////////////////////
    class LZDLL_API cSoundMgr
    {
    public:
        typedef std::map<std::string, LPDSBuffer> SoundMap;
        typedef SoundMap::iterator SoundIter;
        typedef std::list<RefPtr<Sound>> SoundPool;

        static cSoundMgr* instance();

        void release();

        IDirectSound8* getSoundDevice(void);

        SoundPtr loadSound(LPCSTR szName);

        /*停止所有播放。*/
        void stopAll();

        static bool isSoundPlaying(LPDSBuffer buffer);

    protected:

        LPDSBuffer loadSound_(LPCSTR szName);

        LPDSBuffer copySound_(IDirectSoundBuffer8* pSource);

        cSoundMgr();

        ~cSoundMgr();

        void setCacheSize(int size) { m_cacheSize = size; }

        Sound* addToCache(Sound *sound);

        Sound* findUnuse(std::string name);

        bool isSoundUnuse(Sound *sound);

        void updateCache();

    protected:

        IDirectSound8   *m_dsound;

        SoundMap        m_sounds;
        SoundPool       m_cache;
        int             m_cacheSize;

        static cSoundMgr *m_instance;
    };

} // end namespace Lazy
