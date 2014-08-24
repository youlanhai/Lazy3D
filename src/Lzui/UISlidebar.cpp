
#include "stdafx.h"
#include "UISlidebar.h"
#include "UIFactory.h"
#include "TypeParser.h"

namespace Lazy
{
    template<typename T>
    void clamp(T & v, T low, T high)
    {
        if (v < low) v = low;
        else if (v > high) v = high;
    }

    //////////////////////////////////////////////////////////////////////////
    Slider::Slider()
        : m_pSlidebar(nullptr)
    {
        setDragable(true);
    }

    bool Slider::onEvent(const SEvent & event)
    {
        return Button::onEvent(event);
    }

    void Slider::onDrag(const CPoint & delta, const CPoint & point)
    {
        if (!m_pSlidebar) return;

        int i = m_pSlidebar->getVertical() ? 1 : 0;
        m_pSlidebar->slideDelta(delta[i]);
    }

    void Slider::setSize(int w, int h)
    {
        Button::setSize(w, h);

        if (m_pSlidebar) m_pSlidebar->layoutSlider();
    }


    //////////////////////////////////////////////////////////////////////////
    Slidebar::Slidebar()
        : m_bVertical(false)
        , m_rate(0.0f)
        , m_slideStep(0.05f)
        , m_slider(nullptr)
    {
        setDragable(false);

        setSlider(new Slider());
    }

    void Slidebar::setSlider(Slider * silider)
    {
        if (silider == m_slider) return;

        if (m_slider)
        {
            delChild(m_slider);
            m_slider->m_pSlidebar = nullptr;
        }

        m_slider = silider;

        if (m_slider)
        {
            m_slider->m_pSlidebar = this;
            addChild(silider);
        }

        layoutSlider();
    }

    void Slidebar::setSliderSize(int w, int h)
    {
        if (m_slider)
            m_slider->setSize(w, h);
    }

    CPoint Slidebar::getSliderSize() const
    {
        CPoint size;
        if (m_slider) size = m_slider->getSize();

        return size;
    }

    bool Slidebar::onEvent(const SEvent & event)
    {
        if (event.isMouseEvent() && event.mouseEvent.event == EME_MOUSE_WHEEL)
        {
            onMouseWheel(event.mouseEvent.wheel, CPoint(event.mouseEvent.x, event.mouseEvent.y));
            return true;
        }

        return Window::onEvent(event);
    }

    void Slidebar::setVertical(bool vertical)
    {
        m_bVertical = vertical;
        layoutSlider();
    }

    void Slidebar::layoutSlider()
    {
        if (!m_slider) return;

        int i = m_bVertical ? 1 : 0;
        int j = (i + 1) & 1;

        CPoint pos;
        pos[i] = int(m_rate * getSlideRange());
        pos[j] = (m_size[j] - m_slider->m_size[j]) / 2;

        m_slider->setPosition(pos.x, pos.y);
    }

    void Slidebar::setRate(float percent)
    {
        m_rate = percent;
        clamp(m_rate, 0.0f, 1.0f);

        layoutSlider();
    }

    void Slidebar::setSize(int w, int h)
    {
        Window::setSize(w, h);
        layoutSlider();
    }


    void Slidebar::onMouseWheel(float wheel, CPoint pt)
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

    int Slidebar::getSlideDelta() const
    {
        return int(m_slideStep * getSlideRange());
    }

    int Slidebar::getSlideRange() const
    {
        int i = m_bVertical ? 1 : 0;
        int range = m_size[i];

        if (m_slider)
            range -= m_slider->m_size[i];

        if (range <= 0) range = 1;
        return range;
    }

    void Slidebar::slideBackward(void)
    {
        slideDelta(-getSlideDelta());
    }

    void Slidebar::slideForward(void)
    {
        slideDelta(getSlideDelta());
    }

    void Slidebar::slideDelta(int dt)
    {
        m_rate += float(dt) / getSlideRange();
        clamp(m_rate, 0.0f, 1.0f);

        layoutSlider();

        onSlide();
    }

    void Slidebar::onSlide()
    {
#ifdef ENABLE_SCRIPT
        m_self.call_method_quiet("onSlide", m_rate);
#endif
    }


    bool Slidebar::setProperty(LZDataPtr config, const tstring & key, LZDataPtr val)
    {
        if (key == L"vertical")
            setVertical(val->asBool());
        else if (key == L"slideStep")
            setSlideStep(val->asFloat());
        else if (key == L"sliderSize")
        {
            CPoint pt = TypeParser::parsePoint(val->asString());
            setSliderSize(pt.x, pt.y);
        }
        else
            return Window::setProperty(config, key, val);
        return true;
    }

}//namespace Lazy