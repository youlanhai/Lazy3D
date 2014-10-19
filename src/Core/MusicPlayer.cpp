
#include "stdafx.h"
#include "LRCLoader.h"
#include "MusicPlayer.h"
#include "App.h"

#pragma comment(lib, "audiere.lib")

namespace Lazy
{

//using namespace audiere;

    MusicPlayer::MusicPlayer(void)
    {
        m_bLRCEnable = false;
        m_nCommand = 0;     //控制命令。1-播放，2-停止。
        m_bFade = true;        //是否淡入淡出
        m_nFadeTime = 1.0f;    //淡化时间(ms)
        m_nElapseTime = 0;  //已经淡化时间
        m_fOldVolum = 1.0f;
    }

    MusicPlayer::~MusicPlayer(void)
    {
    }

    bool MusicPlayer::loadMusic(LPCSTR szName)
    {
        if (!m_pDevice)
        {
            m_pDevice = audiere::OpenDevice();
            if(!m_pDevice)
            {
                return false;
            }
        }
        if (!m_pLrcLoader)
        {
            m_pLrcLoader = new LRCLoader();
        }
        m_pStream = audiere::OpenSound(m_pDevice, szName, true);
        if (!(m_pStream))
        {
            return false;
        }
        //加载歌词
        char szLRC[256];
        strcpy_s(szLRC, 256, szName);
        char *p = strstr(szLRC, ".mp3");
        if(p == NULL)
        {
            m_bLRCEnable = false;
        }
        else
        {
            strcpy_s(p, 5, ".lrc");
            m_bLRCEnable = m_pLrcLoader->load(szLRC);
            if (!m_bLRCEnable)
            {
                //XWRITE_LOGA("加载歌词%s失败！", szLRC);
            }
        }
        return true;
    }

    void MusicPlayer::play(void)
    {
        m_nCommand = 1;
        m_nElapseTime = 0;
    }
    void MusicPlayer::pause(void)
    {
        m_nCommand = 2;
        m_nElapseTime = 0;
    }
    void MusicPlayer::stop(void)
    {
        m_nCommand = 3;
        m_nElapseTime = 0;
    }

    void MusicPlayer::update(float elapse)
    {
        if (!m_pDevice || !m_pStream)
        {
            return;
        }
        if (m_bLRCEnable)
        {
            /*
            pos=m_pStream->getPosition()获得是帧位置，已知MP3歌曲的频率是
            44.1kHZ，那么pos/44100就是已播放的时间(s)。
            */
            int pos = int(m_pStream->getPosition() / 44.1) + 200; //单位ms
            std::vector<LRC> & vec = m_pLrcLoader->getVec();
            int index = m_pLrcLoader->getCurIndex();
            if (index < 0)
            {
                index = 0;
            }
            /*
            pos位置要正好置于ti与ti+1之间，此时应当播放ti时的歌词。
            */
            while(index > 0)
            {
                if(vec[index].time < pos)
                {
                    break;
                }
                else --index;
            }
            while((UINT)index < vec.size() - 1)
            {
                if(vec[index + 1].time > pos)
                {
                    break;
                }
                else ++index;
            }

            if(index >= 0 && (UINT)index < vec.size())
            {
                m_pLrcLoader->setCurIndex(index);
                LRC *pl = m_pLrcLoader->atCur();
                if(pl != NULL)
                {
                    m_strText = pl->lyrics;
                }
            }
            else m_strText = "~end~";
        }
        if (m_bFade)
        {
            if (1 == m_nCommand)//播放
            {
                if (m_nElapseTime == 0)
                {
                    m_pStream->setVolume(0.0f);
                    m_pStream->play();
                }
                //淡入处理
                if (m_nElapseTime <= m_nFadeTime)
                {
                    m_nElapseTime += elapse;
                    float v = m_nElapseTime / (float)m_nFadeTime * m_fOldVolum;
                    if (v > 1.0f)
                    {
                        v = 1.0f;
                    }
                    m_pStream->setVolume(v);
                }
                else//淡入结束
                {
                    m_nElapseTime = 0;
                    m_nCommand = 0;
                }
            }
            else if(2 == m_nCommand)//暂停
            {
                static float oldVolum = 1.0f;
                if (m_nElapseTime == 0)
                {
                    oldVolum = m_pStream->getVolume();
                }
                //淡出处理
                if (m_nElapseTime <= m_nFadeTime)
                {
                    m_nElapseTime += elapse;
                    float v = (1.0f - m_nElapseTime / (float)m_nFadeTime) * oldVolum;
                    if (v < 0.0f)
                    {
                        v = 0.0f;
                    }
                    m_pStream->setVolume(v);
                }
                else//淡出结束
                {
                    m_pStream->stop();
                    m_nElapseTime = 0;
                    m_nCommand = 0;
                }
            }
            else if (3 == m_nCommand)//停止
            {
                static float oldVolum = 1.0f;
                if (m_nElapseTime == 0)
                {
                    oldVolum = m_pStream->getVolume();
                }
                //淡出处理
                if (m_nElapseTime <= m_nFadeTime)
                {
                    m_nElapseTime += elapse;
                    float v = (1.0f - m_nElapseTime / (float)m_nFadeTime) * oldVolum;
                    if (v < 0.0f)
                    {
                        v = 0.0f;
                    }
                    m_pStream->setVolume(v);
                }
                else//淡出结束
                {
                    m_pStream->stop();
                    m_pStream->reset();
                    m_nElapseTime = 0;
                    m_nCommand = 0;
                }
            }
        }
        else
        {
            if (1 == m_nCommand)//播放
            {
                m_pStream->play();
            }
            else if (2 == m_nCommand)//暂停
            {
                m_pStream->stop();
            }
            else if (3 == m_nCommand)//停止
            {
                m_pStream->stop();
                m_pStream->reset();
            }
        }
    }

#if 0
    void MusicPlayer::render(HDC hDC)
    {
        if(!m_bVisible || !m_bLRCEnable)
        {
            return;
        }
#if 0
        if(!m_bTransparent)
        {
            //用背景颜色绘制背景
            HBRUSH brush = CreateSolidBrush(m_crBackColor);
            HPEN pen = CreatePen(PS_SOLID, 2, m_crBackColor);
            HBRUSH oldBrush = (HBRUSH)SelectObject(hDC, brush);
            HPEN olePen = (HPEN)SelectObject(hDC, pen);

            Rectangle(hDC, m_rect.x, m_rect.y, m_rect.x + m_rect.w, m_rect.y + m_rect.h);

            SelectObject(hDC, oldBrush);
            SelectObject(hDC, olePen);
            DeleteObject(brush);
            DeleteObject(pen);
        }
#endif
        {
            //绘制标题
            RECT rc ;
            rc.left = 0;
            rc.top = 0;
            rc.right = m_rect.w;
            rc.bottom = m_rect.h;

            HFONT font = createFontEx(m_nFontHeight, m_szFontName);
            HBITMAP backBmp = CreateCompatibleBitmap(hDC, m_rect.w, m_rect.h);

            {
                //绘制背景文字
                HDC backDC = CreateCompatibleDC(hDC);
                SelectObject(backDC, backBmp);
                SelectObject(backDC, font);
                SetTextColor(backDC, m_crBackColor);
                SetBkMode(backDC, TRANSPARENT);

                DrawText(backDC, m_strText.c_str(), m_strText.size(),
                         &rc, m_dwDrawStyle);

                TransparentBlt(hDC, m_rect.x, m_rect.y, m_rect.w, m_rect.h,
                               backDC, 0, 0, m_rect.w, m_rect.h, GetPixel(backDC, 0, 0));

                DeleteDC(backDC);
            }
            int pos = int(m_pStream->getPosition() / 44.1) + 200; //单位ms
            std::vector<LRC> & vec = m_pLrcLoader->getVec();
            int index = m_pLrcLoader->getCurIndex();
            int range = 0;//vec[index+1].time - vec[index].time;
            if (index < 0)
            {
                index = 0;
            }
            if((UINT)index >= vec.size() - 1)
            {
                index = vec.size() - 1;
                range = int(m_pStream->getLength() / 44.1) - vec[index].time;
            }
            else
            {
                range = vec[index + 1].time - vec[index].time;
            }
            {
                //绘制前景文字
                HDC foreDC = CreateCompatibleDC(hDC);
                SelectObject(foreDC, backBmp);
                SelectObject(foreDC, font);
                SetTextColor(foreDC, m_crForeColor);
                SetBkMode(foreDC, TRANSPARENT);

                DrawText(foreDC, m_strText.c_str(), m_strText.size(),
                         &rc, m_dwDrawStyle);

                TEXTMETRIC tm;
                GetTextMetrics(foreDC, &tm);
                int textWidth = m_strText.size() * tm.tmAveCharWidth;
                pos -= vec[index].time;
                int w = textWidth * pos / range;
                int x = (m_rect.w - textWidth) / 2;
                TransparentBlt(hDC, x, m_rect.y, w, m_rect.h,
                               foreDC, x, 0, w, m_rect.h, GetPixel(foreDC, 0, 0));

                DeleteDC(foreDC);
            }

            DeleteObject(font);
            DeleteObject(backBmp);
        }
    }
#endif

//是否正在播放
    bool MusicPlayer::isPlaying(void)
    {
        SafeCheck(m_pStream, false);
        return m_pStream->isPlaying();
    }

//重置。将播放位置置于开头。
    void MusicPlayer::reset(void)
    {
        SafeCheck(m_pStream, RT_VOID);
        m_pStream->reset();
    } ;

//设置重复播放
    void MusicPlayer::setRepeat(bool repeat)
    {
        SafeCheck(m_pStream, RT_VOID);
        m_pStream->setRepeat(repeat);
    }
    bool MusicPlayer::getRepeat(void)
    {
        SafeCheck(m_pStream, false);
        return m_pStream->getRepeat();
    }


//设置音量
    void MusicPlayer::setVolume(float volume)
    {
        m_fOldVolum = volume;
        if (m_fOldVolum < 0.0f)
        {
            m_fOldVolum = 0.0f;
        }
        else if(m_fOldVolum > 1.0f)
        {
            m_fOldVolum = 1.0f;
        }
        m_pStream->setVolume(m_fOldVolum);
    }

    float MusicPlayer::getVolume(void)
    {
        SafeCheck(m_pStream, 0.0f);
        return m_pStream->getVolume();
    }

//设置音效平衡
    void MusicPlayer::setPan(float pan)
    {
        SafeCheck(m_pStream, RT_VOID);
        m_pStream->setPan(pan);
    }

    float MusicPlayer::getPan(void)
    {
        SafeCheck(m_pStream, 0.0f);
        return m_pStream->getPan();
    }

    void MusicPlayer::setPitchShift(float shift)
    {
        SafeCheck(m_pStream, RT_VOID);
        m_pStream->setPitchShift(shift);
    }

    float MusicPlayer::getPitchShift(void)
    {
        SafeCheck(m_pStream, 0.0f);
        return m_pStream->getPitchShift();
    }

    bool MusicPlayer::isSeekable(void)
    {
        SafeCheck(m_pStream, false);
        return m_pStream->isSeekable();
    }

//获得音频长度。单位为帧。
    int MusicPlayer::getLength(void)
    {
        SafeCheck(m_pStream, 0);
        return m_pStream->getLength();
    }

//设置播放位置。单位为帧。
    void MusicPlayer::setPosition(int position)
    {
        SafeCheck(m_pStream, RT_VOID);
        m_pStream->setPosition(position);
    }

    int MusicPlayer::getPosition(void)
    {
        SafeCheck(m_pStream, 0);
        return m_pStream->getPosition();
    }


} // end namespace Lazy
