#pragma once

#include "LzpyUIBase.h"

namespace Lzpy
{

    class LzpyLabel : public LzpyControl
    {
        LZPY_DEF(LzpyLabel, LzpyControl);
    public:
        LzpyLabel();

        CLabel * getUI() { return m_control.cast<CLabel>(); }

        LZPY_DEF_GET(align, getUI()->getAlign);
        LZPY_DEF_SET(align, getUI()->setAlign, uint32);

        LZPY_DEF_GET(mutiline, getUI()->canMutiLine);
        LZPY_DEF_SET(mutiline, getUI()->enableMutiLine, bool);

        LZPY_DEF_GET(lineSpace, getUI()->getLineSpace);
        LZPY_DEF_SET(lineSpace, getUI()->setLineSpace, int);

        LZPY_DEF_GET(maxWidth, getUI()->getMaxWidth);
        LZPY_DEF_SET(maxWidth, getUI()->setMaxWidth, int);

        LZPY_DEF_GET(textLines, getUI()->getTextLines);
        LZPY_DEF_GET(textSize, getTextSize);

        object getTextSize();
    };


    class LzpyForm : public LzpyControl
    {
        LZPY_DEF(LzpyForm, LzpyControl);
    public:
        LzpyForm();

        CForm * getUI(){ return m_control.cast<CForm>(); }

        LZPY_DEF_GET(enableClip, getUI()->canClip);
        LZPY_DEF_SET(enableClip, getUI()->enableClip, bool);
    };


    class LzpySlidebar : public LzpyForm
    {
        LZPY_DEF(LzpySlidebar, LzpyForm);
    public:
        LzpySlidebar();

        CSlidebar * getUI() { return m_control.cast<CSlidebar>(); }

        LZPY_DEF_GET(vertical, getUI()->getVertical);
        LZPY_DEF_SET(vertical, getUI()->setVertical, bool);

        LZPY_DEF_GET(rate, getUI()->getRate);
        LZPY_DEF_SET(rate, getUI()->setRate, bool);

        LZPY_DEF_GET(slideStep, getUI()->getSlideStep);
        LZPY_DEF_SET(slideStep, getUI()->setSlideStep, float);

        LZPY_DEF_GET(sliderSize, getSliderSize);
        LZPY_DEF_SET(sliderSize, setSliderSize, tuple);

        void setSliderSize(tuple size);
        tuple getSliderSize();
    };




    class LzpyButton : public LzpyControl
    {
        LZPY_DEF(LzpyButton, LzpyControl);
    public:
        LzpyButton();

    };

    class LzpySelect : public LzpyButton
    {
        LZPY_DEF(LzpySelect, LzpyButton);
    public:
        LzpySelect();

        CCheckBox * getUI() { return m_control.cast<CCheckBox>(); }

        LZPY_DEF_GET(check, getUI()->getCheck);
        LZPY_DEF_SET(check, getUI()->setCheck, bool);
    };


    class LzpyEditorCtl : public LzpyControl
    {
        LZPY_DEF(LzpyEditorCtl, LzpyControl);
    public:
        LzpyEditorCtl();

        CEditorCtl * getUI(){ return m_control.cast<CEditorCtl>(); }

        LZPY_DEF_GET(edgeSize, getUI()->getEdgeSize);
        LZPY_DEF_SET(edgeSize, getUI()->setEdgeSize, int);
    };

    class LzpyImage : public LzpyControl
    {
        LZPY_DEF(LzpyImage, LzpyControl);
    public:
        LzpyImage();

    };

    class LzpyEdit : public LzpyControl
    {
        LZPY_DEF(LzpyEdit, LzpyControl);
    public:
        LzpyEdit();

        CEdit * getUI(){ return m_control.cast<CEdit>(); }

        LZPY_DEF_GET(mutiline, getUI()->isMutiLine);
        LZPY_DEF_SET(mutiline, getUI()->setMutiLine, bool);

    };

    class LzpyUIProxy : public LzpyControl
    {
        LZPY_DEF(LzpyUIProxy, LzpyControl);
    public:
        LzpyUIProxy();

        UIProxy * getUI(){ return m_control.cast<UIProxy>(); }

        LZPY_DEF_GET(host, getHost);
        LZPY_DEF_METHOD(loadHost);


        object getHost();

    };

}// end namespace Lzpy

