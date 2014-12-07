#pragma once

#include "LPyUIBase.h"

namespace Lzpy
{

    class LzpyLabel : public LzpyControl
    {
        LZPY_DEF(LzpyLabel);
    public:
        LzpyLabel();

        Label * getUI() { return m_control.cast<Label>(); }

        LZPY_DEF_GET(text, getUI()->getText);
        LZPY_DEF_SET(text, getUI()->setText, tstring);

        LZPY_DEF_GET(font, getUI()->getFont);
        LZPY_DEF_SET(font, getUI()->setFont, tstring);

        LZPY_DEF_GET(color, getUI()->getTextColor);
        LZPY_DEF_SET(color, getUI()->setTextColor, uint32);

        LZPY_DEF_GET(textAlign, getUI()->getAlign);
        LZPY_DEF_SET(textAlign, getUI()->setAlign, uint32);

        LZPY_DEF_GET(mutiline, getUI()->getMutiLine);
        LZPY_DEF_SET(mutiline, getUI()->setMutiLine, bool);

        LZPY_DEF_GET(lineSpace, getUI()->getLineSpace);
        LZPY_DEF_SET(lineSpace, getUI()->setLineSpace, int);

        LZPY_DEF_GET(textLines, getUI()->getTextLines);
        LZPY_DEF_GET(textSize, getTextSize);

        object getTextSize();
    };


    class LzpyForm : public LzpyControl
    {
        LZPY_DEF(LzpyForm);
    public:
        LzpyForm();

        Window * getUI() { return m_control.cast<Window>(); }

        LZPY_DEF_GET(enableClip, getUI()->getClipable);
        LZPY_DEF_SET(enableClip, getUI()->setClipable, bool);
    };


    class LzpySlidebar : public LzpyForm
    {
        LZPY_DEF(LzpySlidebar);
    public:
        LzpySlidebar();

        Slidebar * getUI() { return m_control.cast<Slidebar>(); }

        LZPY_DEF_GET(vertical, getUI()->getVertical);
        LZPY_DEF_SET(vertical, getUI()->setVertical, bool);

        LZPY_DEF_GET(rate, getUI()->getRate);
        LZPY_DEF_SET(rate, getUI()->setRate, bool);

        LZPY_DEF_GET(slideStep, getUI()->getSlideStep);
        LZPY_DEF_SET(slideStep, getUI()->setSlideStep, float);

        LZPY_DEF_GET(sliderSize, getSliderSize);
        LZPY_DEF_SET(sliderSize, setSliderSize, tuple);

        LZPY_DEF_METHOD_0(slideForward);
        LZPY_DEF_METHOD_0(slideBackward);
        LZPY_DEF_METHOD_1(slideByWheel);

        void setSliderSize(tuple size);
        tuple getSliderSize();
    };




    class LzpyButton : public LzpyControl
    {
        LZPY_DEF(LzpyButton);
    public:
        LzpyButton();

        Button * getUI() { return m_control.cast<Button>(); }

        LZPY_DEF_GET(textAlign, getUI()->getTextAlign);
        LZPY_DEF_SET(textAlign, getUI()->setTextAlign, uint32);

        LZPY_DEF_GET(colorFadable, getUI()->getColorFadable);
        LZPY_DEF_SET(colorFadable, getUI()->setColorFadable, bool);

        LZPY_DEF_GET(posMovable, getUI()->getPosMovable);
        LZPY_DEF_SET(posMovable, getUI()->setPosMovable, bool);
    };

    class LzpySelect : public LzpyButton
    {
        LZPY_DEF(LzpySelect);
    public:
        LzpySelect();

        CheckBox * getUI() { return m_control.cast<CheckBox>(); }

        LZPY_DEF_GET(check, getUI()->getCheck);
        LZPY_DEF_SET(check, getUI()->setCheck, bool);
    };


    class LzpyEditorCtl : public LzpyControl
    {
        LZPY_DEF(LzpyEditorCtl);
    public:
        LzpyEditorCtl();

        EditorCtl * getUI() { return m_control.cast<EditorCtl>(); }

        LZPY_DEF_GET(edgeSize, getUI()->getEdgeSize);
        LZPY_DEF_SET(edgeSize, getUI()->setEdgeSize, int);
    };

    class LzpyImage : public LzpyControl
    {
        LZPY_DEF(LzpyImage);
    public:
        LzpyImage();

        Image * getUI() { return m_control.cast<Image>(); }

        LZPY_DEF_GET(image, getUI()->getImage);
        LZPY_DEF_SET(image, getUI()->setImage, tstring);

        LZPY_DEF_GET(color, getUI()->getColor);
        LZPY_DEF_SET(color, getUI()->setColor, uint32);
    };

    class LzpyEdit : public LzpyControl
    {
        LZPY_DEF(LzpyEdit);
    public:
        LzpyEdit();

        Edit * getUI() { return m_control.cast<Edit>(); }

        LZPY_DEF_GET(mutiline, getUI()->isMutiLine);
        LZPY_DEF_SET(mutiline, getUI()->setMutiLine, bool);

    };

    class LzpyUIProxy : public LzpyControl
    {
        LZPY_DEF(LzpyUIProxy);
    public:
        LzpyUIProxy();

        UIProxy * getUI() { return m_control.cast<UIProxy>(); }

        LZPY_DEF_GET(host, getHost);
        LZPY_DEF_METHOD(loadHost);


        object getHost();

    };

}// end namespace Lzpy

