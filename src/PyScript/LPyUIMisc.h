#pragma once

#include "LPyUIBase.h"

namespace Lazy
{

    class PyLabel : public PyWidget
    {
        LZPY_DEF(PyLabel);
    public:
        PyLabel();

        Label * getUI() { return (Label *) m_object; }

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


    class PyWindow : public PyWidget
    {
        LZPY_DEF(PyWindow);
    public:
        PyWindow();

        Window * getUI() { return (Window *)m_object; }

        LZPY_DEF_GET(enableClip, getUI()->getClipable);
        LZPY_DEF_SET(enableClip, getUI()->setClipable, bool);
    };


    class PySlidebar : public PyWindow
    {
        LZPY_DEF(PySlidebar);
    public:
        PySlidebar();

        Slidebar * getUI() { return (Slidebar *) m_object; }

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




    class PyButton : public PyWidget
    {
        LZPY_DEF(PyButton);
    public:
        PyButton();

        Button * getUI() { return (Button *)m_object; }

        LZPY_DEF_GET(textAlign, getUI()->getTextAlign);
        LZPY_DEF_SET(textAlign, getUI()->setTextAlign, uint32);

        LZPY_DEF_GET(colorFadable, getUI()->getColorFadable);
        LZPY_DEF_SET(colorFadable, getUI()->setColorFadable, bool);

        LZPY_DEF_GET(posMovable, getUI()->getPosMovable);
        LZPY_DEF_SET(posMovable, getUI()->setPosMovable, bool);
    };

    class PyCheckBox : public PyButton
    {
        LZPY_DEF(PyCheckBox);
    public:
        PyCheckBox();

        CheckBox * getUI() { return (CheckBox *)m_object; }

        LZPY_DEF_GET(check, getUI()->getCheck);
        LZPY_DEF_SET(check, getUI()->setCheck, bool);
    };


    class PyEditorCtl : public PyWidget
    {
        LZPY_DEF(PyEditorCtl);
    public:
        PyEditorCtl();

        EditorCtl * getUI() { return (EditorCtl *)m_object; }

        LZPY_DEF_GET(edgeSize, getUI()->getEdgeSize);
        LZPY_DEF_SET(edgeSize, getUI()->setEdgeSize, int);
    };

    class PyImage : public PyWidget
    {
        LZPY_DEF(PyImage);
    public:
        PyImage();

        Image * getUI() { return (Image *)m_object; }

        LZPY_DEF_GET(image, getUI()->getImage);
        LZPY_DEF_SET(image, getUI()->setImage, tstring);

        LZPY_DEF_GET(color, getUI()->getColor);
        LZPY_DEF_SET(color, getUI()->setColor, uint32);
    };

    class PyEdit : public PyWidget
    {
        LZPY_DEF(PyEdit);
    public:
        PyEdit();

        Edit * getUI() { return (Edit *)m_object; }

        LZPY_DEF_GET(mutiline, getUI()->isMutiLine);
        LZPY_DEF_SET(mutiline, getUI()->setMutiLine, bool);

    };

    class PyUIProxy : public PyWidget
    {
        LZPY_DEF(PyUIProxy);
    public:
        PyUIProxy();

        UIProxy * getUI() { return (UIProxy *)m_object; }

        LZPY_DEF_GET(host, getHost);
        LZPY_DEF_METHOD(loadHost);


        object getHost();

    };

}// end namespace Lazy

