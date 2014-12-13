#include "stdafx.h"
#include "LPyUIMisc.h"
#include "LPyLzd.h"
#include "LPyConsole.h"

namespace Lazy
{

    ///////////////////////////////////////////////////////////////////
    LZPY_CLASS_BEG(PyLabel);
    LZPY_GETSET(lineSpace);
    LZPY_GETSET(textAlign);
    LZPY_GETSET(mutiline);
    LZPY_GET(textLines);
    LZPY_GET(textSize);
    LZPY_CLASS_END();


    PyLabel::PyLabel()
    {
    }

    LZPY_IMP_INIT_LUI(PyLabel);

    object PyLabel::getTextSize()
    {
        const CSize & s = getUI()->getTextSize();
        return build_tuple(s.cx, s.cy);
    }

    ///////////////////////////////////////////////////////////////////
    LZPY_CLASS_BEG(PyWindow)
    LZPY_GETSET(enableClip);
    LZPY_CLASS_END();

    PyWindow::PyWindow()
    {

    }

    LZPY_IMP_INIT_LUI(PyWindow);


    ///////////////////////////////////////////////////////////////////

    ///////////////////////////////////////////////////////////////////
    LZPY_CLASS_BEG(PySlidebar)
    LZPY_GETSET(vertical);
    LZPY_GETSET(rate);
    LZPY_GETSET(sliderSize);
    LZPY_GETSET(slideStep);
    LZPY_METHOD_0(slideForward);
    LZPY_METHOD_0(slideBackward);
    LZPY_METHOD_1(slideByWheel);
    LZPY_CLASS_END();

    PySlidebar::PySlidebar()
    {

    }

    LZPY_IMP_INIT_LUI(PySlidebar);

    void PySlidebar::setSliderSize(tuple size)
    {
        if (size.size() != 2)
            throw(python_error("setSliderSize: tuple 2 needed!"));

        CSize sz;
        size.parse_tuple(&sz.cx, &sz.cy);
        getUI()->setSliderSize(sz.cx, sz.cy);
    }

    tuple PySlidebar::getSliderSize()
    {
        CPoint size = getUI()->getSliderSize();
        return build_tuple(size.x, size.y);
    }

    LZPY_IMP_METHOD_0(PySlidebar, slideForward)
    {
        getUI()->slideForward();
        return none_object;
    }

    LZPY_IMP_METHOD_0(PySlidebar, slideBackward)
    {
        getUI()->slideBackward();
        return none_object;
    }

    LZPY_IMP_METHOD_1(PySlidebar, slideByWheel)
    {
        float v;
        if (!parse_object(v, value)) return null_object;

        if (v < 0.0f)
        {
            getUI()->slideForward();
        }
        else
        {
            getUI()->slideBackward();
        }

        return none_object;
    }

    ///////////////////////////////////////////////////////////////////
    LZPY_CLASS_BEG(PyButton);
    LZPY_GETSET(textAlign);
    LZPY_GETSET(colorFadable);
    LZPY_GETSET(posMovable);
    LZPY_CLASS_END();

    PyButton::PyButton()
    {

    }

    LZPY_IMP_INIT_LUI(PyButton);

    ///////////////////////////////////////////////////////////////////
    LZPY_CLASS_BEG(PyCheckBox)
    LZPY_GETSET(check);
    LZPY_CLASS_END();

    PyCheckBox::PyCheckBox()
    {

    }

    LZPY_IMP_INIT_LUI(PyCheckBox);

    ///////////////////////////////////////////////////////////////////
    LZPY_CLASS_BEG(PyEditorCtl)
    LZPY_GETSET(edgeSize);
    LZPY_CLASS_END();

    PyEditorCtl::PyEditorCtl()
    {

    }

    LZPY_IMP_INIT_LUI(PyEditorCtl)

    ///////////////////////////////////////////////////////////////////
    LZPY_CLASS_BEG(PyImage);

    LZPY_CLASS_END();

    PyImage::PyImage()
    {

    }

    LZPY_IMP_INIT_LUI(PyImage)

    ///////////////////////////////////////////////////////////////////
    LZPY_CLASS_BEG(PyEdit)
    LZPY_GETSET(mutiline);
    LZPY_CLASS_END();

    PyEdit::PyEdit()
    {

    }

    LZPY_IMP_INIT_LUI(PyEdit)

    ///////////////////////////////////////////////////////////////////
    LZPY_CLASS_BEG(PyUIProxy)
    LZPY_GET(host);
    LZPY_METHOD(loadHost);
    LZPY_CLASS_END();

    PyUIProxy::PyUIProxy()
    {

    }

    LZPY_IMP_INIT_LUI(PyUIProxy);

    LZPY_IMP_METHOD(PyUIProxy, loadHost)
    {
        tstring filename;
        if (!arg.parse_tuple(&filename)) return null_object;

        return build_object(getUI()->loadHost(filename));
    }

    object PyUIProxy::getHost()
    {
        Widget * p = getUI()->getHost();
        if (!p || !p->getScript()) return none_object;

        return object(p->getScript());
    }

    ///////////////////////////////////////////////////////////////////
    //函数定义
    ///////////////////////////////////////////////////////////////////
    LZPY_DEF_FUN(uiroot)
    {
        return PyScriptProxy::New(getGUIMgr());
    }

    LZPY_DEF_FUN(param2Position)
    {
        size_t param;
        if (!PyArg_ParseTuple(arg, "I", &param)) return NULL;

        CPoint pt(param);
        return Py_BuildValue("ii", pt.x, pt.y);
    }

    LZPY_DEF_FUN(position2Param)
    {
        CPoint pt;
        if (!PyArg_ParseTuple(arg, "ii", &pt.x, &pt.y)) return NULL;

        return Py_BuildValue("I", pt.toLParam());
    }

    LZPY_DEF_FUN(isVkDown)
    {
        DWORD vk;
        if (!PyArg_ParseTuple(arg, "k", &vk)) return NULL;

        bool isDown = Widget::isVKDown(vk);

        return PyBool_FromLong(isDown);
    }

    LZPY_DEF_FUN(createUI)
    {
        wchar_t * type;
        if (!PyArg_ParseTuple(arg, "u", &type))
            return NULL;

        Widget * p = uiFactory()->create(type);
        return PyScriptProxy::New(p);
    }

    ///////////////////////////////////////////////////////////////////
    //函数导出
    ///////////////////////////////////////////////////////////////////


    void exportUI(const char *module)
    {
        LZPY_REGISTER_CLASS(Widget, PyWidget);
        LZPY_REGISTER_CLASS(Label, PyLabel);
        LZPY_REGISTER_CLASS(Window, PyWindow);
        LZPY_REGISTER_CLASS(Button, PyButton);
        LZPY_REGISTER_CLASS(CheckBox, PyCheckBox);
        LZPY_REGISTER_CLASS(Slidebar, PySlidebar);
        LZPY_REGISTER_CLASS(Image, PyImage);
        LZPY_REGISTER_CLASS(Edit, PyEdit);
        LZPY_REGISTER_CLASS(UIProxy, PyUIProxy);
        LZPY_REGISTER_CLASS(EditorCtl, PyEditorCtl);
        LZPY_REGISTER_CLASS(ConsoleOutput, PyConsoleOutput);

        LZPY_FUN(uiroot);
        LZPY_FUN(createUI);
        LZPY_FUN(param2Position);
        LZPY_FUN(position2Param);
        LZPY_FUN(isVkDown);
    }

}// end namespace Lazy
