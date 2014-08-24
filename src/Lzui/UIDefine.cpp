#include "stdafx.h"
#include "UIDefine.h"
#include "UIWidget.h"

namespace Lazy
{

    namespace
    {
        tstring & defaultFont()
        {
            static tstring s_defaultFont = L"def|17";
            return s_defaultFont;
        }

        static FontPtr s_defaultFontPtr;
    }

    void setDefaultFont(const tstring & font)
    {
        defaultFont() = font;

        s_defaultFontPtr = FontMgr::instance()->getFont(font);
    }

    const tstring & getDefaultFont()
    {
        return defaultFont();
    }

    FontPtr getDefaultFontPtr()
    {
        return s_defaultFontPtr;
    }


    //////////////////////////////////////////////////////////////////////////
    WidgetChildren::WidgetChildren(Widget *pOwner)
        : m_pOwner(pOwner)
    {

    }

    void WidgetChildren::update(float elapse)
    {
        lock();
        Widget * p;
        for (iterator it = begin(); it != end(); ++it)
        {
            p = *it;
            if (p->getVisible() && p->getParent() == m_pOwner)
                p->update(elapse);
        }
        unlock();
    }

    void WidgetChildren::render(IUIRender* pDevice)
    {
        lock();
        Widget * p;
        for (reverse_iterator it = rbegin(); it != rend(); ++it)
        {
            p = *it;
            if( p->getVisible() && (p->getLayer() == nullptr || p->getLayer() == m_pOwner) )
                p->render(pDevice);
        }
        unlock();
    }

    //////////////////////////////////////////////////////////////////////////
    ISprite::ISprite()
    {

    }

    ISprite::~ISprite()
    {

    }

    void ISprite::getRect(CRect & rc) const
    {
        rc.left = m_position.x;
        rc.top = m_position.y;
        rc.right = rc.left + m_size.cx;
        rc.bottom = rc.top + m_size.cy;
    }

    CRect ISprite::getRect() const
    {
        CRect rc;
        getRect(rc);
        return rc;
    }

    //////////////////////////////////////////////////////////////////////////
    CColorFade::CColorFade(void)
    {
        m_crCur = m_crDest = 0;
        m_time = m_elapse = 0.0f;

    }
    void CColorFade::reset(COLORREF crCur, COLORREF crDest, float time/*=-1.0f*/)
    {
        m_crCur = crDest;
        m_time = time;
        setDestColor(crDest);
    }

    void CColorFade::update(float fElapse)
    {
        if ((m_elapse <= 0.0f) || (m_crCur == m_crDest))
        {
            m_crCur = m_crDest;
            return;
        }

        float v = 0.0f;
        BYTE *ps = (BYTE*) &m_crCur;
        for (int i = 0; i < 3; ++i)
        {
            v = ps[i] + m_v[i] * fElapse;
            if (v < 0.0f)
            {
                v = 0.0f;
            }
            else if (v > 255.0f)
            {
                v = 255.0f;
            }
            ps[i] = BYTE(v);
        }
        m_elapse -= fElapse;
    }

    void CColorFade::setDestColor(COLORREF cr)
    {
        m_crDest = cr;
        m_elapse = m_time;
        BYTE *pd = (BYTE*) &cr;
        BYTE *ps = (BYTE*) &m_crCur;
        for (int i = 0; i < 3; ++i)
        {
            m_v[i] = (pd[i] - ps[i]) / m_time;
        }
    }

    ///////////////////////////////////////////////////////////////////

    int distance2(POINT* a, POINT* b)
    {
        int x = a->x - b->x;
        int y = a->y - b->y;
        return x * x + y * y;
    }

    float distance(POINT* a, POINT* b)
    {
        return (float) sqrt((float) distance2(a, b));
    }

    CPosFade::CPosFade(void)
    {
        m_vx = 0;
        m_vy = 0;
        m_fadeOver = 1;
        m_speed = 0.0f;
    }

    void CPosFade::reset(CPoint cur, CPoint dest, float speed/* = -1.0f*/)
    {
        m_cur = cur;
        setDest(dest, speed);
    }

    void CPosFade::setDest(CPoint dest, float speed/* = -1.0f*/)
    {
        m_dest = dest;
        if (speed > 0.0f)
        {
            m_speed = speed;
        }
        float t = distance(&m_dest, &m_cur) / m_speed;
        m_vx = (m_dest.x - m_cur.x) / t;
        m_vy = (m_dest.y - m_cur.y) / t;
        m_dirvx = m_dest.x > m_cur.x;
        m_dirvy = m_dest.y > m_cur.y;
        m_fadeOver = -1;
    }

    void CPosFade::update(float fElapse)
    {
        if (m_fadeOver >= 0 || m_dest == m_cur)
        {
            m_fadeOver = 1;
            return;
        }
        m_cur.x += (LONG) (m_vx * fElapse);
        m_cur.y += (LONG) (m_vy * fElapse);
        if (m_dest.x > m_cur.x != m_dirvx || m_dest.y > m_cur.y != m_dirvy)
        {
            m_fadeOver = 0;
            m_cur = m_dest;
        }
    }


    namespace misc
    {
        bool readPosition(CPoint & pos, LZDataPtr ptr, const tstring & tag)
        {
            LZDataPtr p = ptr->read(tag);
            if (!p) return false;

            return p->formatTo(L"%d %d", &pos.x, &pos.y);
        }

        bool writePosition(const CPoint & pos, LZDataPtr ptr, const tstring & tag)
        {
            LZDataPtr p = ptr->newOne(tag, L"");
            p->formatFrom(L"%d %d", pos.x, pos.y);

            ptr->addChild(p);
            return true;
        }

        bool readSize(CSize & size, LZDataPtr ptr, const tstring & tag)
        {
            LZDataPtr p = ptr->read(tag);
            if (!p) return false;

            return p->formatTo(L"%d %d", &size.cx, &size.cy);
        }

        bool writeSize(const CSize & size, LZDataPtr ptr, const tstring & tag)
        {
            LZDataPtr p = ptr->newOne(tag, L"");
            p->formatFrom(L"%d %d", size.cx, size.cy);

            ptr->addChild(p);
            return true;
        }

        bool readVector2(Vector2 & vec, LZDataPtr ptr, const tstring & tag)
        {
            LZDataPtr p = ptr->read(tag);
            if (!p) return false;

            return p->formatTo(L"%f %f", &vec.x, &vec.y);
        }

        bool writeVector2(Vector2 & vec, LZDataPtr ptr, const tstring & tag)
        {
            LZDataPtr p = ptr->newOne(tag, L"");
            p->formatFrom(L"%f %f", vec.x, vec.y);

            ptr->addChild(p);
            return true;
        }
    }




}//end namespace Lazy