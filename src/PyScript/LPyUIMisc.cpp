﻿#include "stdafx.h"
#include "LPyUIMisc.h"
#include "LPyLzd.h"
#include "LPyConsole.h"

namespace Lzpy
{

    ///////////////////////////////////////////////////////////////////
    LZPY_CLASS_BEG(LzpyLabel);
    LZPY_GETSET(lineSpace);
    LZPY_GETSET(textAlign);
    LZPY_GETSET(mutiline);
    LZPY_GET(textLines);
    LZPY_GET(textSize);
    LZPY_CLASS_END();


    LzpyLabel::LzpyLabel()
    {
    }

    LZPY_IMP_INIT_LUI(LzpyLabel, Label);

    object LzpyLabel::getTextSize()
    {
        const CSize & s = getUI()->getTextSize();
        return build_tuple(s.cx, s.cy);
    }

    ///////////////////////////////////////////////////////////////////
    LZPY_CLASS_BEG(LzpyForm)
    LZPY_GETSET(enableClip);
    LZPY_CLASS_END();

    LzpyForm::LzpyForm()
    {

    }

    LZPY_IMP_INIT_LUI(LzpyForm, Form);


    ///////////////////////////////////////////////////////////////////

    ///////////////////////////////////////////////////////////////////
    LZPY_CLASS_BEG(LzpySlidebar)
    LZPY_GETSET(vertical);
    LZPY_GETSET(rate);
    LZPY_GETSET(sliderSize);
    LZPY_GETSET(slideStep);
    LZPY_METHOD_0(slideForward);
    LZPY_METHOD_0(slideBackward);
    LZPY_METHOD_1(slideByWheel);
    LZPY_CLASS_END();

    LzpySlidebar::LzpySlidebar()
    {

    }

    LZPY_IMP_INIT_LUI(LzpySlidebar, Slidebar);

    void LzpySlidebar::setSliderSize(tuple size)
    {
        if (size.size() != 2)
            throw(python_error("setSliderSize: tuple 2 needed!"));

        CSize sz;
        size.parse_tuple(&sz.cx, &sz.cy);
        getUI()->setSliderSize(sz.cx, sz.cy);
    }

    tuple LzpySlidebar::getSliderSize()
    {
        CPoint size = getUI()->getSliderSize();
        return build_tuple(size.x, size.y);
    }

    LZPY_IMP_METHOD_0(LzpySlidebar, slideForward)
    {
        getUI()->slideForward();
        return none_object;
    }

    LZPY_IMP_METHOD_0(LzpySlidebar, slideBackward)
    {
        getUI()->slideBackward();
        return none_object;
    }

    LZPY_IMP_METHOD_1(LzpySlidebar, slideByWheel)
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
    LZPY_CLASS_BEG(LzpyButton);
    LZPY_GETSET(textAlign);
    LZPY_GETSET(colorFadable);
    LZPY_GETSET(posMovable);
    LZPY_CLASS_END();

    LzpyButton::LzpyButton()
    {

    }

    LZPY_IMP_INIT_LUI(LzpyButton, Button);

    ///////////////////////////////////////////////////////////////////
    LZPY_CLASS_BEG(LzpySelect)
    LZPY_GETSET(check);
    LZPY_CLASS_END();

    LzpySelect::LzpySelect()
    {

    }

    LZPY_IMP_INIT_LUI(LzpySelect, Check);

    ///////////////////////////////////////////////////////////////////
    LZPY_CLASS_BEG(LzpyEditorCtl)
    LZPY_GETSET(edgeSize);
    LZPY_CLASS_END();

    LzpyEditorCtl::LzpyEditorCtl()
    {

    }

    LZPY_IMP_INIT_LUI(LzpyEditorCtl, EditorCtl)

    ///////////////////////////////////////////////////////////////////
    LZPY_CLASS_BEG(LzpyImage);

    LZPY_CLASS_END();

    LzpyImage::LzpyImage()
    {

    }

    LZPY_IMP_INIT_LUI(LzpyImage, Image)

    ///////////////////////////////////////////////////////////////////
    LZPY_CLASS_BEG(LzpyEdit)
    LZPY_GETSET(mutiline);
    LZPY_CLASS_END();

    LzpyEdit::LzpyEdit()
    {

    }

    LZPY_IMP_INIT_LUI(LzpyEdit, Edit)

    ///////////////////////////////////////////////////////////////////
    LZPY_CLASS_BEG(LzpyUIProxy)
    LZPY_GET(host);
    LZPY_METHOD(loadHost);
    LZPY_CLASS_END();

    LzpyUIProxy::LzpyUIProxy()
    {

    }

    LZPY_IMP_INIT_LUI(LzpyUIProxy, Proxy);

    LZPY_IMP_METHOD(LzpyUIProxy, loadHost)
    {
        tstring filename;
        if (!arg.parse_tuple(&filename)) return null_object;

        return build_object(getUI()->loadHost(filename));
    }

    object LzpyUIProxy::getHost()
    {
        Widget * p = getUI()->getHost();
        if (!p || !p->getSelf()) return none_object;

        return object(p->getSelf());
    }

    ///////////////////////////////////////////////////////////////////
    //函数定义
    ///////////////////////////////////////////////////////////////////

    static LzpyControl * s_root = nullptr;
    static object s_uiFactoryMethod;

    LZPY_DEF_FUN(root)
    {
        return xincref(s_root);
    }

    LZPY_DEF_FUN(regUIFactory)
    {
        PyObject *method;
        if (!PyArg_ParseTuple(arg, "O", &method)) return NULL;

        s_uiFactoryMethod = object(method);
        Py_RETURN_NONE;
    }

    object createPythonUI(LZDataPtr config)
    {
        assert(s_uiFactoryMethod && "The factory method must not be null!");

        return s_uiFactoryMethod.call(make_object(config));
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

    ///////////////////////////////////////////////////////////////////
    //函数导出
    ///////////////////////////////////////////////////////////////////


    LZPY_MODULE_BEG(lui);
    LZPY_REGISTER_CLASS(IControl, LzpyControl);
    LZPY_REGISTER_CLASS(Label, LzpyLabel);
    LZPY_REGISTER_CLASS(Form, LzpyForm);
    LZPY_REGISTER_CLASS(Button, LzpyButton);
    LZPY_REGISTER_CLASS(CheckBox, LzpySelect);
    LZPY_REGISTER_CLASS(Slidebar, LzpySlidebar);
    LZPY_REGISTER_CLASS(Image, LzpyImage);
    LZPY_REGISTER_CLASS(Edit, LzpyEdit);
    LZPY_REGISTER_CLASS(UIProxy, LzpyUIProxy);
    LZPY_REGISTER_CLASS(EditorCtl, LzpyEditorCtl);
    LZPY_REGISTER_CLASS(ConsoleOutput, PyConsoleOutput);

    LZPY_FUN(root);
    LZPY_FUN(param2Position);
    LZPY_FUN(position2Param);
    LZPY_FUN(regUIFactory);
    LZPY_FUN(isVkDown);

    LZPY_MODULE_END();


    static Widget* pyEditorUICreateFun(Widget *pParent, LZDataPtr config)
    {
        object pyChild = createPythonUI(config);
        if (!pyChild) return nullptr;

        //加入缓存池，用于保持引用计数。防止子控件析构。
        (pyChild.cast<LzpyControl>())->setManaged(true);

        //加载控件数据，并加入到panel中。
        return (pyChild.cast<LzpyControl>())->m_control.get();
    }


    namespace _py_lui
    {
        class ResLoader : public LzpyResInterface
        {
        public:

            void init() override
            {
                s_root = helper::new_instance_ex<LzpyControl>();

                s_root->m_control = getGUIMgr();
                s_root->m_control->setSelf(object_base(s_root));

               // setEditorUICreateFun(pyEditorUICreateFun);
            }

            void fini() override
            {
                Py_XDECREF(s_root);
                s_root = nullptr;

                s_uiFactoryMethod = null_object;
            }
        };

        static ResLoader s_resLoader;
    }

}// end namespace Lzpy