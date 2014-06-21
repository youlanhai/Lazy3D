
#include "stdafx.h"

#include <mmsystem.h>
#pragma comment(lib,"dxguid")
#pragma comment(lib,"dsound")

#include "Sound.h"
#include "App.h"

#include "../utility/Utility.h"

namespace Lazy
{
//////////////////////////////////////////////////////////////////////////
    Sound::Sound(std::string name, LPDSBuffer buffer)
        : m_name(name), m_pBuffer(buffer)
    {
        if (m_pBuffer)
        {
            m_pBuffer->AddRef();
        }
    }

    Sound::~Sound(void)
    {
        SafeRelease(m_pBuffer);
    }

    /*播放。*/
    bool Sound::play()
    {
        stop();
        if(FAILED(m_pBuffer->Play(0, 0, 0)))
        {
            return false;
        }
        return true;
    }

    /** 暂停*/
    bool Sound::pause()
    {
        return stop();
    }

    /*停止播放。*/
    bool Sound::stop()
    {
        if(FAILED(m_pBuffer->Stop()))
        {
            return false;
        }
        return false;
    }

    /*是否正在播放。*/
    bool Sound::isPlaying()
    {
        return cSoundMgr::isSoundPlaying(m_pBuffer);
    }

    /*设置频率。*/
    bool Sound::setFrequncy(long)
    {
        return false;
    }

    /*设置平衡。*/
    bool Sound::setPan(long)
    {
        return false;
    }

    /*设置音量。*/
    bool Sound::setVolum(long)
    {
        return false;
    }

//////////////////////////////////////////////////////////////////////////

    /*static*/ cSoundMgr* cSoundMgr::m_instance = NULL;

    /*static*/ cSoundMgr* cSoundMgr::instance()
    {
        if (NULL == m_instance)
        {
            m_instance = new cSoundMgr();
        }
        return m_instance;
    }

    bool cSoundMgr::isSoundPlaying(LPDSBuffer buffer)
    {
        if (NULL == buffer)
        {
            return false;
        }
        DWORD status;
        if(FAILED(buffer->GetStatus(&status)))
        {
            return false;
        }
        if(status == DSBSTATUS_LOOPING || status == DSBSTATUS_PLAYING)
        {
            return true;
        }
        return false;
    }

    void cSoundMgr::release()
    {
        SafeDelete(m_instance);
    }

    IDirectSound8* cSoundMgr::getSoundDevice(void)
    {
        if(NULL == m_dsound)
        {
            if(FAILED(DirectSoundCreate8(NULL, &m_dsound, NULL)))
            {
                WRITE_LOG(_T("创建dsound8设备失败"));
                return NULL;
            }
            if(FAILED(m_dsound->SetCooperativeLevel(getApp()->getHWnd(), DSSCL_NORMAL)))
            {
                WRITE_LOG(_T("设置dsound8协作等级失败"));
                return NULL;
            }
        }
        return m_dsound;
    }

    SoundPtr cSoundMgr::loadSound(LPCSTR szName)
    {
        Sound *pSound = findUnuse(szName);
        if (pSound)
        {
            return pSound;
        }

        LPDSBuffer buffer = NULL;
        SoundIter it = m_sounds.find(szName);
        if (it == m_sounds.end())
        {
            buffer = loadSound_(szName);
        }
        else
        {
            buffer = it->second;
        }
        if (NULL == buffer)
        {
            return NULL;
        }
        buffer = copySound_(buffer); //仅使用拷贝的，不使用源音。
        if (NULL == buffer)
        {
            return NULL;
        }
        return addToCache(new Sound(szName, buffer));
    }


    /** 加载资源。*/
    IDirectSoundBuffer8* cSoundMgr::loadSound_(LPCSTR szName)
    {
        XWRITE_LOGA("sound : try to load sound [%s] from file.", szName);
        if (NULL == getSoundDevice())
        {
            return NULL;
        }
        HMMIO hwave;
        MMCKINFO parent, child;
        WAVEFORMATEX wmfx;

        parent.ckid = (FOURCC)0;
        parent.cksize = 0;
        parent.dwDataOffset = 0;
        parent.fccType = (FOURCC)0;
        parent.dwFlags = 0;
        child = parent;
        if((hwave = mmioOpenA(const_cast<LPSTR>(szName), NULL, MMIO_READ | MMIO_ALLOCBUF)) == NULL)
        {
            return false;
        }
        parent.fccType = mmioFOURCC('W', 'A', 'V', 'E');
        if(mmioDescend(hwave, &parent, NULL, MMIO_FINDRIFF))
        {
            mmioClose(hwave, 0);
            return false;
        }
        child.ckid = mmioFOURCC('f', 'm', 'm', ' ');
        if(mmioDescend(hwave, &child, &parent, 0))
        {
            mmioClose(hwave, 0);
            return false;
        }
        if(mmioRead(hwave, (char*)&wmfx, sizeof(wmfx)) != sizeof(wmfx))
        {
            mmioClose(hwave, 0);
            return false;
        }
        if(wmfx.wFormatTag != WAVE_FORMAT_PCM)
        {
            mmioClose(hwave, 0);
            return false;
        }
        if(mmioAscend(hwave, &child, 0))
        {
            mmioClose(hwave, 0);
            return false;
        }
        child.ckid = mmioFOURCC('d', 'a', 't', 'a');
        if(mmioDescend(hwave, &child, &parent, MMIO_FINDCHUNK))
        {
            mmioClose(hwave, 0);
            return false;
        }
        UCHAR *snd_buffer = new UCHAR[sizeof(UCHAR)*child.cksize];
        if(mmioRead(hwave, (char*)snd_buffer, child.cksize) == -1)
        {
            mmioClose(hwave, 0);
            return false;
        }
        mmioClose(hwave, 0);

        //创建缓冲区
        DWORD dwBufferSize = 100000;
        DSBUFFERDESC dsbd;
        memset(&dsbd, 0, sizeof(dsbd));
        dsbd.dwSize = sizeof(dsbd);
        dsbd.dwFlags = DSBCAPS_CTRLFREQUENCY | DSBCAPS_CTRLPAN | DSBCAPS_CTRLVOLUME
                       | DSBCAPS_STATIC | DSBCAPS_LOCSOFTWARE;
        if(dwBufferSize > child.cksize)
            dwBufferSize = child.cksize;
        dsbd.dwBufferBytes = dwBufferSize;
        dsbd.lpwfxFormat = &wmfx;

        IDirectSoundBuffer8 *pBuffer8 = NULL;
        if(FAILED(getSoundDevice()->CreateSoundBuffer(&dsbd,
                  (LPDIRECTSOUNDBUFFER*)&pBuffer8, NULL)))
        {
            return false;
        }

        //锁定缓冲区
        UCHAR *audio_ptr1, *audio_ptr2;
        DWORD audio_len1, audio_len2;
        if(FAILED(pBuffer8->Lock(0, child.cksize, (void**)&audio_ptr1, &audio_len1,
                                 (void**)&audio_ptr2, &audio_len2, DSBLOCK_ENTIREBUFFER)))
        {
            return false;
        }
        //向缓冲区写入数据
        memcpy(audio_ptr1, snd_buffer, audio_len1);
        memcpy(audio_ptr2, snd_buffer + audio_len1, audio_len2);
        //解除锁定
        pBuffer8->Unlock(audio_ptr1, audio_len1, audio_ptr2, audio_len2);

        delete [] snd_buffer;

        return pBuffer8;
    }

    IDirectSoundBuffer8* cSoundMgr::copySound_(IDirectSoundBuffer8* pSource)
    {
        if (NULL == pSource || NULL == getSoundDevice())
        {
            return NULL;
        }
        IDirectSoundBuffer8 *pBuffer8 = NULL;
        if(FAILED(getSoundDevice()->DuplicateSoundBuffer(pSource, (IDirectSoundBuffer**)&pBuffer8)))
        {
            return NULL;
        }
        return pBuffer8;
    }

    cSoundMgr::cSoundMgr()
    {
        m_dsound = NULL;
        m_cacheSize = 100;
    }

    cSoundMgr::~cSoundMgr()
    {
        m_cache.clear();
        for (SoundIter it = m_sounds.begin(); it != m_sounds.end(); ++it)
        {
            SafeRelease(it->second);
        }
        m_sounds.clear();
        if(m_dsound)
        {
            SafeRelease(m_dsound);
            WRITE_LOG(_T("dsound8设备已释放"));
        }
    }

    Sound* cSoundMgr::addToCache(Sound *sound)
    {
        updateCache();
        m_cache.push_back(sound);
        return sound;
    }

    Sound* cSoundMgr::findUnuse(std::string name)
    {
        for (SoundPool::iterator it = m_cache.begin();
                it != m_cache.end();
                ++it)
        {
            if ((*it)->getName() == name)
            {
                if(isSoundUnuse(it->get()))
                {
                    debugMessageA("sound : find one unuse sound [%s] from cache.", name.c_str());
                    return it->get();
                }
            }
        }
        return NULL;
    }

    void cSoundMgr::updateCache()
    {
        if (m_cache.size() < (size_t)m_cacheSize)
        {
            return;
        }
        for (SoundPool::iterator it = m_cache.begin();
                it != m_cache.end();
                ++it)
        {
            if (isSoundUnuse(it->get()))
            {
                it = m_cache.erase(it);
                if (m_cache.size() < (size_t)m_cacheSize)
                {
                    break;
                }
#if 0
                if (it == m_cache.end())
                {
                    break;
                }
#endif
            }
        }
    }

    bool cSoundMgr::isSoundUnuse(Sound *sound)
    {
        return sound->getRef() == 1 && !sound->isPlaying();
    }
} // end namespace Lazy
