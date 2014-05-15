#pragma once

#include "renderres.h"

typedef IDirectSoundBuffer8*  LPDSBuffer;


//////////////////////////////////////////////////////////////////////////
class LZDLL_API cSound : public IBase
{
public:
    cSound(std::string name, LPDSBuffer buffer);

    ~cSound(void);

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

    std::string getName(void){ return m_name; }
protected:
    std::string     m_name;
    LPDSBuffer      m_pBuffer;
};

typedef RefPtr<cSound> SoundPtr;

//////////////////////////////////////////////////////////////////////////
class LZDLL_API cSoundMgr
{
public:
    typedef std::map<std::string, LPDSBuffer> SoundMap;
    typedef SoundMap::iterator SoundIter;
    typedef std::list<RefPtr<cSound>> SoundPool;

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

    void setCacheSize(int size){ m_cacheSize = size; }

    cSound* addToCache(cSound *sound);

    cSound* findUnuse(std::string name);

    bool isSoundUnuse(cSound *sound);

    void updateCache();

protected:

    IDirectSound8   *m_dsound;

    SoundMap        m_sounds;
    SoundPool       m_cache;
    int             m_cacheSize;

    static cSoundMgr *m_instance;
};
