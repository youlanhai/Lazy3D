#include "stdafx.h"
#include "LPyMisc.h"
#include "LPyEntity.h"
#include "LPyPhysics.h"

namespace LazyPy
{

    LAZYPY_IMP("SkyBox", PySkyBox, "Lazy");
    LAZYPY_BEGIN_EXTEN(PySkyBox);

    LAZYPY_METHOD_1(show);
    LAZYPY_METHOD_0(visible);
    LAZYPY_METHOD_0(toggle);
    LAZYPY_METHOD(setRange);

    LAZYPY_GETSET(source);
    LAZYPY_GETSET(image);

    LAZYPY_END_EXTEN();

    PySkyBox::PySkyBox()
    {
        m_sky = getApp()->getSkyBox();
    }

    PySkyBox::~PySkyBox()
    {
        m_sky->setSource(nullptr);
    }

    LAZYPY_IMP_INIT(PySkyBox)
    {
        PyErr_SetString(PyExc_RuntimeError, "Can't create an instance of Lazy.SkyBox");
        return false;
    }

    LAZYPY_IMP_METHOD_1(PySkyBox, show)
    {
        m_sky->show(fromPyObject<bool>(value));

        Py_RETURN_NONE;
    }

    LAZYPY_IMP_METHOD_0(PySkyBox, visible)
    {
        return PyBool_FromLong(m_sky->visible());
    }
    LAZYPY_IMP_METHOD_0(PySkyBox, toggle)
    {
        m_sky->toggle();
        Py_RETURN_NONE;
    }


    void PySkyBox::setSource(const object & v)
    {
        if (m_source == v) return;
        if (v && !PyEntity_Check(v.get())) return;

        m_source = v;

        iEntity *pSource = nullptr;
        if (m_source)
        {
            pSource = m_source->m_entity.get();
        }

        m_sky->setSource(pSource);
    }

    LAZYPY_IMP_METHOD(PySkyBox, setRange)
    {
        PyObject * pMin, *pMax;
        if (!PyArg_ParseTuple(arg, "OO", &pMin, &pMax)) return nullptr;

        Physics::Vector3 vecMin, vecMax;
        if (!v3FromPyObject(vecMin, pMin)) return nullptr;

        if (!v3FromPyObject(vecMax, pMax)) return nullptr;

        m_sky->setSkyRange(vecMin, vecMax);

        Py_RETURN_NONE;
    }


    ////////////////////////////////////////////////////////////////////
    LAZYPY_IMP("Topboard", PyTopboard, "Lazy");
    LAZYPY_BEGIN_EXTEN(PyTopboard);
    
    LAZYPY_GETSET(text);
    LAZYPY_GETSET(color);
    LAZYPY_GETSET(biasHeight);
    LAZYPY_GETSET(source);

    LAZYPY_END_EXTEN();

    PyTopboard::PyTopboard()
    {
        m_topboard = new cTopBoard();
    }

    PyTopboard::~PyTopboard()
    {
        m_topboard->setSource(nullptr);
        m_topboard = nullptr;
    }

    LAZYPY_IMP_INIT(PyTopboard)
    {
        wchar_t *text, *font;
        DWORD cr;
        float biasHeight;

        if (!PyArg_ParseTuple(arg, "uuIf", &text, &font, &cr, &biasHeight)) return false;

        m_topboard->create(text, font, cr, biasHeight);
        return true;
    }

    void PyTopboard::setSource(const object & value)
    {
        if (m_source == value) return;
        if (value && !PyEntity_Check(value.get())) return;

        m_source = value;

        iEntity *pSource = nullptr;
        if (m_source)
        {
            pSource = m_source->m_entity.get();
        }

        m_topboard->setSource(pSource);
    }
    
    ////////////////////////////////////////////////////////////////////

    class PyCallObj : public Lazy::callObj
    {
    public:
        PyCallObj(float time, PyObject * pCall, PyObject *pArg)
            : Lazy::callObj(time)
            , m_pCall(pCall)
            , m_pArg(pArg)
        {
            assert(pCall && "PyCallObj");

            Py_INCREF(m_pCall);
            
            if (m_pArg) Py_INCREF(m_pArg);
            else m_pArg = PyTuple_New(0);
        }

        ~PyCallObj()
        {
            Py_DECREF(m_pCall);
            Py_DECREF(m_pArg);
        }

        virtual void onDead() override
        {
            PyObject *ret = PyObject_Call(m_pCall, m_pArg, NULL);
            Py_XDECREF(ret);
        }

    private:
        PyObject *m_pCall;
        PyObject *m_pArg;
    };

    static PySkyBox *s_pSkyBox = nullptr;

    LAZYPY_DEF_FUN_0(getSkyBox)
    {
        return toPyObject(s_pSkyBox);
    }

    LAZYPY_DEF_FUN(callback)
    {
        float time;
        PyObject *pFun;
        PyObject *pFunArg = nullptr;

        if (!PyArg_ParseTuple(arg, "fO|O", &time, &pFun, &pFunArg)) return nullptr;

        if (!PyCallable_Check(pFun))
        {
            PyErr_SetString(PyExc_TypeError, "argument 2 is not callable!");
            return nullptr;
        }

        if (pFunArg && !PyTuple_Check(pFunArg))
        {
            PyErr_SetString(PyExc_TypeError, "argument 3 must be a tuple!");
            return nullptr;
        }

        size_t id = Lazy::CallbackMgr::instance()->add(new PyCallObj(time, pFun, pFunArg));
        return toPyObject(id);
    }

    LAZYPY_DEF_FUN_1(cancelCallback)
    {
        size_t id = 0;
        fromPyObject(id, value);

        Lazy::CallbackMgr::instance()->remove(id);

        Py_RETURN_NONE;
    }

    namespace _py_misc
    {
        class ResLoader : public LazyPyResInterface
        {
        public:
            void init() override
            {
                s_pSkyBox = helper::new_instance_ex<PySkyBox>();
            }

            void fini() override
            {
                Py_DECREF(s_pSkyBox);
                s_pSkyBox = nullptr;
            }
        };

        static ResLoader s_resLoader;


        LAZYPY_BEGIN_FUN(Lazy);

        LAZYPY_FUN_0(getSkyBox);
        LAZYPY_FUN(callback);
        LAZYPY_FUN_1(cancelCallback);

        LAZYPY_END_FUN();
    }


    LAZYPY_IMP_MODULE(Lazy);
}