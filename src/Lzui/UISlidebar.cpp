
#include "stdafx.h"
#include "UISlidebar.h"
#include "UIFactory.h"

namespace Lazy
{
    template<typename T>
    void clamp(T & v, T low, T high)
    {
        if (v < low) v = low;
        else if (v > high) v = high;
    }

    //////////////////////////////////////////////////////////////////////////
    CSlider::CSlider()
        : m_pSlidebar(nullptr)
    {
        enableDrag(true);
    }

    bool CSlider::onEvent(const SEvent & event)
    {
        if (event.isMouseEvent() && event.mouseEvent.event == EME_MOUSE_WHEEL) return false;

        return CButton::onEvent(event);
    }

    void CSlider::onDrag(const CPoint & delta, const CPoint & point)
    {
        if (!m_pSlidebar) return;

        int i = m_pSlidebar->getVertical() ? 1 : 0;
        m_pSlidebar->slideDelta(delta[i]);
    }

    void CSlider::setSize(int w, int h)
    {
        CButton::setSize(w, h);

        if (m_pSlidebar) m_pSlidebar->layoutSlider();
    }


    //////////////////////////////////////////////////////////////////////////
    CSlidebar::CSlidebar()
        : m_bVertical(false)
        , m_rate(0.0f)
        , m_slideStep(0.05f)
    {
        enableDrag(false);
        enableLimitInRect(false);
        setBgColor(0x7f000000);

        setSlider(new CSlider());
    }

    void CSlidebar::setSlider(SliderPtr silider)
    {
        if (silider == m_slider) return;

        if (m_slider)
        {
            delChild(m_slider.get());
            m_slider->m_pSlidebar = nullptr;
        }

        m_slider = silider;

        if (m_slider)
        {
            m_slider->m_pSlidebar = this;
            addChild(silider.get());
        }

        layoutSlider();
    }

    void CSlidebar::setSliderSize(int w, int h)
    {
        if (m_slider)
            m_slider->setSize(w, h);
    }

    CSize CSlidebar::getSliderSize() const
    {
        CSize size;
        if (m_slider) size = m_slider->getSize();

        return size;
    }

    bool CSlidebar::onEvent(const SEvent & event)
    {
        if (event.isMouseEvent() && event.mouseEvent.event == EME_MOUSE_WHEEL)
        {
            onMouseWheel(event.mouseEvent.wheel, CPoint(event.mouseEvent.x, event.mouseEvent.y));
            return IControl::onEvent(event);
        }

        return CForm::onEvent(event);
    }

    void CSlidebar::setVertical(bool vertical)
    {
        m_bVertical = vertical;
        layoutSlider();
    }

    void CSlidebar::layoutSlider()
    {
        if (!m_slider) return;

        int i = m_bVertical ? 1 : 0;
        int j = (i + 1) & 1;
     
        CPoint pos;
        pos[i] = int(m_rate * getSlideRange());
        pos[j] = (m_size[j] - m_slider->m_size[j]) / 2;

        m_slider->setPosition(pos.x, pos.y);
    }

    void CSlidebar::setRate(float percent)
    {
        m_rate = percent;
        clamp(m_rate, 0.0f, 1.0f);

        layoutSlider();
    }

    void CSlidebar::setSize(int w, int h)
    {
        CForm::setSize(w, h);
        layoutSlider();
    }


    void CSlidebar::onMouseWheel(float wheel, CPoint pt)
    {
        if (!m_slider) return;

        if (wheel < 0.0f)
        {
            slideForward();
        }
        else
        {
            slideBackward();
        }
    }

    int CSlidebar::getSlideDelta() const
    {
        return int(m_slideStep * getSlideRange());
    }

    int CSlidebar::getSlideRange() const
    {
        int i = m_bVertical ? 1 : 0;
        int range = m_size[i];

        if (m_slider)
            range -= m_slider->m_size[i];

        if (range <= 0) range = 1;
        return range;
    }

    void CSlidebar::slideBackward(void)
    {
        slideDelta(-getSlideDelta());
    }

    void CSlidebar::slideForward(void)
    {
        slideDelta(getSlideDelta());
    }

    void CSlidebar::slideDelta(int dt)
    {
        m_rate += float(dt) / getSlideRange();
        clamp(m_rate, 0.0f, 1.0f);

        layoutSlider();

        onSlide();
    }

    void CSlidebar::onSlide()
    {
#ifdef ENABLE_SCRIPT
        Lzpy::object(m_pSelf).call_method_quiet("onSlide", m_rate);
#endif
    }


}//namespace Lazy