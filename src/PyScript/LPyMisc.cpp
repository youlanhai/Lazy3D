#include "stdafx.h"
#include "LPyMisc.h"
#include "LPyEntity.h"
#include "LPyPhysics.h"
#include "LPyMap.h"

namespace Lzpy
{

    LZPY_CLASS_BEG(PySkyBox);
    LZPY_METHOD(setRange);
    LZPY_GETSET(visible);
    LZPY_GETSET(source);
    LZPY_GETSET(image);
    LZPY_CLASS_END();

    PySkyBox::PySkyBox()
    {
        m_sky = new SkyBox();
    }

    PySkyBox::~PySkyBox()
    {
        m_sky->setSource(nullptr);
    }

    LZPY_IMP_INIT(PySkyBox)
    {
        return false;
    }

    PyEntity * PySkyBox::getPySource()
    {
        return m_source.cast<PyEntity>();
    }

    void PySkyBox::setSource(const object & v)
    {
        if (m_source == v)
            return;

        m_source = none_object;
        m_sky->setSource(nullptr);

        if (v.is_none())
        {

        }
        else if (CHECK_INSTANCE(PyEntity, v.get()))
        {
            m_source = v;
            m_sky->setSource(m_source->entity());
        }
    }

    LZPY_IMP_METHOD(PySkyBox, setRange)
    {
        Vector3 vecMin, vecMax;
        if (!arg.parse_tuple(&vecMin, &vecMax))
            return null_object;

        m_sky->setSkyRange(vecMin, vecMax);

        return none_object;
    }


    ////////////////////////////////////////////////////////////////////
    LZPY_CLASS_BEG(PyTopboard);

    LZPY_GETSET(text);
    LZPY_GETSET(color);
    LZPY_GETSET(biasHeight);
    LZPY_GETSET(source);

    LZPY_CLASS_END();

    PyTopboard::PyTopboard()
    {
        m_topboard = new TopBoard();
    }

    PyTopboard::~PyTopboard()
    {
        m_topboard->setSource(nullptr);
        m_topboard = nullptr;
    }

    LZPY_IMP_INIT(PyTopboard)
    {
        wchar_t *text, *font;
        DWORD cr;
        float biasHeight;

        if (!PyArg_ParseTuple(arg, "uuIf", &text, &font, &cr, &biasHeight)) return false;

        m_topboard->create(text, font, cr, biasHeight);
        return true;
    }

    void PyTopboard::setSource(const object & v)
    {
        if (m_source == v)
            return;

        m_source = none_object;
        m_topboard->setSource(nullptr);

        if (v.is_none())
        {

        }
        else if (CHECK_INSTANCE(PyEntity, v.get()))
        {
            m_source = v;
            m_topboard->setSource(m_source->entity());
        }
    }

    ////////////////////////////////////////////////////////////////////

    class PyCallObj : public Lazy::ITimerDelegate
    {
    public:
        PyCallObj(PyObject * pCall, PyObject *pArg)
            : m_func(pCall)
            , m_args(pArg)
        {}


        virtual void onCall()
        {
            m_func.call_python(m_args);
        }

        virtual void onCancel() {}
        virtual void onExit() {}

    private:
        object m_func;
        object m_args;
    };

    LZPY_DEF_FUN(callback)
    {
        float time;
        PyObject *pFun;
        PyObject *pFunArg = nullptr;

        if (!PyArg_ParseTuple(arg, "fO|O", &time, &pFun, &pFunArg)) return nullptr;

        if (!PyCallable_Check(pFun))
        {
            PyErr_SetString(PyExc_TypeError, "argument 2 must callable!");
            return nullptr;
        }

        if (pFunArg && !PyTuple_Check(pFunArg))
        {
            PyErr_SetString(PyExc_TypeError, "argument 3 must be a tuple!");
            return nullptr;
        }

        TTimeHandle id = TimerMgr::instance()->addTimer(time, new PyCallObj(pFun, pFunArg));
        return xincref(build_object(id));
    }

    LZPY_DEF_FUN_1(cancelCallback)
    {
        TTimeHandle id = 0;
        if (!parse_object(id, object(value)))
            return nullptr;

        TimerMgr::instance()->remove(id);
        Py_RETURN_NONE;
    }

    namespace _py_misc
    {
        class ResLoader : public LzpyResInterface
        {
        public:

            void init() override
            {
            }

            void fini() override
            {
            }
        };

        static ResLoader s_resLoader;
    }


    void exportMisc(const char * module)
    {
        LZPY_REGISTER_CLASS(Topboard, PyTopboard);
        LZPY_REGISTER_CLASS(SkyBox, PySkyBox);

        LZPY_FUN(callback);
        LZPY_FUN_1(cancelCallback);
    }
}