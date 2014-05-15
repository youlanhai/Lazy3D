
#include "stdafx.h"
#include "WrapperUI.h"

#include "..\LZGUI\LZGUI.h"
//////////////////////////////////////////////////////////////////////////

template<typename T>
struct WrapperControl : public T, public wrapper<T>
{
    VT2(RT_VOID, void, onButton, int, bool);
    VT2(RT_VOID, void, onScroll, int, float);
    VT3(RT_VOID, void, onMessageProc, UINT, WPARAM, LPARAM);
};

typedef WrapperControl<IControl>    WrapperIControl;
typedef WrapperControl<CPanel>      WapperPanel;
typedef WrapperControl<CForm>       WrapperForm;
typedef WrapperControl<CButton>     WrapperButton;

//////////////////////////////////////////////////////////////////////////

void wrapperUI()
{
    LOG_INFO(L"EXPORT - wrapperUI");

    def("getGUIMgr", getGUIMgr, return_value_policy<reference_existing_object>());

    class_<CColorFade>("cColorFade", init<>())
        .def("reset", &CColorFade::reset)
        .def("setSource", &CColorFade::setSource)
        .def("setDestColor", &CColorFade::setDestColor)
        .def("getCurColor", &CColorFade::getCurColor)
        ;

    class_<WrapperIControl,boost::noncopyable>("iControl")
        .add_property("id", &IControl::getID, &IControl::setID)
        .add_property("text", &IControl::getCaption, &IControl::setCaption)
        .add_property("font", &IControl::getFont, &IControl::setFont)
        .add_property("color", &IControl::getColor, &IControl::setColor)
        .add_property("backColor", &IControl::getBackColor, &IControl::setBackColor)
        .def("init", &IControl::init)
        .def("setBackColor", &IControl::setBackColor)
        .def("getBackColor", &IControl::getBackColor)
        .def("setFont", &IControl::setFont)
        .def("getFont", &IControl::getFont)
        .def("getParent", &IControl::getParent, return_value_policy<reference_existing_object>())
        .def("setParent", &IControl::setParent)
        .def("getImage", &IControl::getImage)
        .def("setImage", &IControl::setImage)
        .def("getPosition", &IControl::getPosition)
        .def("setPosition", &IControl::setPosition)
        .def("getSize", &IControl::getSize)
        .def("setSize", &IControl::setSize)
        .def("show", &IControl::show)
        .def("toggle", &IControl::toggle)
        .def("isVisible", &IControl::isVisible)
        .def("enable", &IControl::enable)
        .def("isEnable", &IControl::isEnable)
        .def("enableTrack", &IControl::enableTrack)
        .def("enableDrag", &IControl::enableDrag)
        .def("canDrag", &IControl::canDrag)
        .def("enableHandleMsg", &IControl::enableHandleMsg)
        .def("canHandleMsg", &IControl::canHandleMsg)
        .def("enableClickTop", &IControl::enableClickTop)
        .def("canClickTop", &IControl::canClickTop)
        .def("onTextChange", &IControl::onTextChange)
        .def("onMessageProc", &IControl::onMessageProc, &WrapperIControl::default_onMessageProc)
        .def("onButton", &IControl::onButton, &WrapperIControl::default_onButton)
        .def("onScroll", &IControl::onScroll, &WrapperIControl::default_onScroll)
        ;

    class_<CLabel, bases<IControl>>("cLabel", init<int, PControl, LPCSTR, int, int>())
        .def("setAlign", &CLabel::setAlign)
        .def("setFont", &CLabel::setFont)
        .def("setMutiLine", &CLabel::setMutiLine)
        ;

    class_<CImage, bases<IControl>>("cImage", init<int, PControl, LPCSTR, int, int, int, int>())
        .def("setTexUV", &CImage::setTexUV)
        ;

    class_<WapperPanel, bases<IControl>, boost::noncopyable>("cPanel")
        .add_property("canHandleSelfMsg", &CPanel::canHandleSelfMsg, &CPanel::enableHandleSelfMsg)
        .def("create", &CPanel::create)
        ;

    class_<GUIManagerBase, bases<CPanel>>("cGUIManager", init<bool>())
        .def("initLayoutPanel", &GUIManagerBase::initLayotPanel)
        .def("bottomPanel", &GUIManagerBase::bottomMostPanel, return_internal_reference<>())
        .def("normalPanel", &GUIManagerBase::normalPanel, return_internal_reference<>())
        .def("topPanel", &GUIManagerBase::topMostPanel, return_internal_reference<>())
        .def("messageProc", &GUIManagerBase::messageProc)
        ;

    class_<WrapperForm, bases<CPanel>, boost::noncopyable>("cForm") 
        .def("create", &CForm::create)
        .def("getImagePtr", &CForm::getImagePtr, return_value_policy<reference_existing_object>())
        ;

    class_<WrapperButton, bases<CForm>, boost::noncopyable>("cButton")
        .def("create", &CButton::create)
        .def("getLabelPtr", &CButton::getLabelPtr, return_value_policy<reference_existing_object>())
        .def("enablePosMove", &CButton::enablePosMove)
        .def("enableColor", &CButton::enableColor)
        .def("setStateColor", &CButton::setStateColor)
        .def("getStateColor", &CButton::getStateColor)
        ;

    typedef WrapperControl<CSilider> WrapperSilider;
    class_<WrapperSilider, bases<CButton>, boost::noncopyable>("cSilider")
        .def("create", &CSilider::create);
    ;

    typedef WrapperControl<CScroll> WrapperScroll;
    class_<WrapperScroll, bases<CForm>, boost::noncopyable>("cScroll")
        .def("create", &CScroll::create)
        .def("setSiliderSize", &CScroll::setSiliderSize)
        .def("setVertical", &CScroll::setVertical)
        .def("setForeImage", &CScroll::setForeImage)
        .def("setRate", &CScroll::setRate)
        .def("getRate", &CScroll::getRate)
        .def("setStep", &CScroll::setStep)
        .def("setVertical", &CScroll::setVertical)
        .def("onScroll", &CScroll::onScroll, &WrapperScroll::default_onScroll)
        .def("getSilider", &CScroll::getSilider, return_value_policy<reference_existing_object>())
        ;
}