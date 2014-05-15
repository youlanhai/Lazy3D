
#include "stdafx.h"
#include "WrapperTool.h"
#include <ostream>

namespace Lazy
{
    class WrapperCallBack : public callObj, public wrapper<callObj>
    {
        object m_fun;

    public:
        WrapperCallBack(float time, object fun)
            : callObj(time)
            , m_fun(fun)
        {
        }

        virtual void onDead(void) override
        {
            m_fun();
        }
    };

    size_t pyCallback(float time, object fun)
    {
        return CallbackMgr::instance()->add(new WrapperCallBack(time, fun));
    }

    class PyOutput
    {
        std::wstring m_cache;
    public:
        PyOutput(){}

        void write(const std::wstring & str)
        {
            if (str == L"\n")
            {
                flush();
            }
            else
            {
                m_cache += str;
            }
        }

        void flush()
        {
            if (m_cache.empty()) return;

            WRITE_LOGW(m_cache);
            m_cache.clear();
        }
    };

}

using namespace Lazy;
using namespace Physics;


void wrapperTool()
{
    LOG_INFO(L"EXPORT - wrapperTool");

    def("callback", Lazy::pyCallback);

    class_<PyOutput>("Output", init<>())
        .def("write", &PyOutput::write)
        .def("flush", &PyOutput::flush)
        ;

    void(Vector3::*vecotr3_normal)() = &Vector3::normalize;
    class_<Vector3>("Vector3", init<>())
        .def(init<float, float, float>())
        .def(init<Vector3>())
        .def_readwrite("x", &Vector3::x)
        .def_readwrite("y", &Vector3::y)
        .def_readwrite("z", &Vector3::z)
        .def("set", &Vector3::set)
        .def("distTo", &Vector3::distTo)
        .def("distToSq", &Vector3::distToSq)
        .def("normal", vecotr3_normal)
        .def("length", &Vector3::length)
        .def("lengthSq", &Vector3::lengthSq)
        .def(self += self)
        .def(self -= self)
        .def(self *= float())
        .def(self /= float())
        .def(self + self)
        .def(self - self)
        .def(self * float())
        .def(self / float())
        .def(self == self)
        .def(self != self)
        .def(self_ns::str(self))
        ;

    class_<D3DXMATRIX>("Matrix4x4", init<>())
        .def(init<D3DXMATRIX>())
        .def(self += self)
        .def(self -= self)
        .def(self *= self)
        .def(self * float())
        .def(self / float())
        .def(self + self)
        .def(self - self)
        .def(self * self)
        .def(self *= float())
        .def(self /= float())
        .def(self == self)
        .def(self != self)
        ;

    class_<CPoint>("Point", init<>())
        .def(init<CPoint>())
        .def(init<LPARAM>())
        .def(init<int, int>())
        .def_readwrite("x", &CPoint::x)
        .def_readwrite("y", &CPoint::y)
        .def("set", &CPoint::set)
        .def("formatLParam", &CPoint::formatLParam)
        .def("toLParam", &CPoint::toLParam)
        .def(self == self)
        .def(self + self)
        .def(self += self)
        .def(self - self)
        .def(self -= self)
        ;

    class_<CSize>("Size", init<>())
        .def(init<int, int>())
        .def(init<CSize>())
        .def_readwrite("w", &CSize::cx)
        .def_readwrite("h", &CSize::cy)
        .def("set", &CSize::set)
        .def(self == self)
        ;

    bool(CRect::*PFisIn)(const CPoint &) const = &CRect::isIn;
    bool(CRect::*PFisIn2)(int, int) const = &CRect::isIn;
    void(CRect::*PFOffset)(int, int) = &CRect::offset;

    class_<CRect>("Rect", init<>())
        .def(init<int, int, int, int>())
        .def(init<CRect>())
        .def(init<CPoint, CPoint>())
        .def(init<CPoint, CSize>())
        .def("set", &CRect::set)
        .def_readwrite("left", &CRect::left)
        .def_readwrite("top", &CRect::top)
        .def_readwrite("right", &CRect::right)
        .def_readwrite("bottom", &CRect::bottom)
        .add_property("width", &CRect::width)
        .add_property("height", &CRect::height)
        .def("offset", PFOffset)
        .def("expand",&CRect::expand)
        .def("isIntersect", &CRect::isIntersect)
        .def("isIn", PFisIn)
        .def("isIn2", PFisIn2)
        ;


    class_<CKeyboard, boost::noncopyable>("Keyboard", no_init)
        .def("isKeyDown", &CKeyboard::isKeyDown)
        .def("isKeyPress", &CKeyboard::isKeyPress)
        .def("isKeyUp", &CKeyboard::isKeyUp)
        ;

    class_<CSceneFog>("Fog", init<>())
        .def("setFogType", &CSceneFog::setFogType)
        .def("setStartEnd", &CSceneFog::setStartEnd)
        .def("setUseRange", &CSceneFog::setUseRange)
        .def("setColor", &CSceneFog::setColor)
        .def("setDensity", &CSceneFog::setDensity)
        .def("show", &CSceneFog::show)
        .def("visible", &CSceneFog::visible)
        .def("toggle", &CSceneFog::toggle)
        ;

}