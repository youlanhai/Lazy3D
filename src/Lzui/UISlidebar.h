#pragma once

#include "UIButton.h"
#include "UIWindow.h"

namespace Lazy
{
    class Slidebar;

    //////////////////////////////////////////////////////////////////////////
    ///滑块
    //////////////////////////////////////////////////////////////////////////
    class LZUI_API Slider : public Button
    {
    public:
        MAKE_UI_HEADER(Slider);

        Slider();

        virtual void setSize(int w, int h) override;

    protected:
        virtual bool onEvent(const SEvent & event) override;
        virtual void onDrag(const CPoint & delta, const CPoint & point) override;

        Slidebar *m_pSlidebar;

        friend class Slidebar;
    };

    //////////////////////////////////////////////////////////////////////////
    //滚动条
    //////////////////////////////////////////////////////////////////////////
    class LZUI_API Slidebar : public Window
    {
    public: 
        MAKE_UI_HEADER(Slidebar);

        Slidebar();

        void setSlider(Slider * sil);
        Slider* getSlider() const { return m_slider; }

        /** 每次滑动，前进的步长*/
        void setSlideStep(float percent) { m_slideStep = percent; }
        float getSlideStep() const { return m_slideStep; }

        /** 是否垂直风格。默认是水平风格。*/
        void setVertical(bool v);
        bool getVertical() const { return m_bVertical; }

        void setRate(float percent);
        float getRate(void) const { return m_rate; }

        void setSliderSize(int w, int h);
        CPoint getSliderSize() const;

        virtual void setSize(int w, int h) override;

        ///滑动。单位为像素。
        void slideDelta(int delta);

        ///后退
        void slideForward(void);

        ///前进
        void slideBackward(void);

        void layoutSlider();

        virtual bool setProperty(LZDataPtr config, const tstring & key, LZDataPtr val) override;

    protected:

        virtual bool onEvent(const SEvent & event) override;

        void onMouseWheel(float wheel, CPoint pt);

        void onSlide();

        int getSlideDelta() const;
        int getSlideRange() const;


        float               m_rate;
        float               m_slideStep;
        Slider*             m_slider; ///<滑块
        bool                m_bVertical;///<是否垂直滚动条
    };


}//namespace Lazy