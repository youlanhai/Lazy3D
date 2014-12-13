#include "stdafx.h"
#include "LPyModel.h"

namespace Lazy
{

    LZPY_CLASS_EXPORT(PyModel)
    {
        LZPY_GET(resource);
        LZPY_GET(height);
        LZPY_GETSET(aabbVisible);

        LZPY_METHOD(playAction);
        LZPY_METHOD_0(stopAction);
        LZPY_METHOD_0(getActionCount);
        LZPY_METHOD_1(getActionName);
        LZPY_METHOD_1(setActionSpeed);
    }

    PyModel::PyModel()
    {
    }

    LZPY_IMP_INIT(PyModel)
    {
        PyErr_SetString(PyExc_RuntimeError, "PyModel can't initialize directly!");
        return false;
    }

    LZPY_IMP_METHOD(PyModel, playAction)
    {
        std::string name;
        bool loop = false;
        if (!arg.parse_tuple_default(1, &name, &loop))
            return null_object;

        model()->playAction(name, loop);
        return none_object;
    }

    LZPY_IMP_METHOD_0(PyModel, stopAction)
    {
        model()->stopAction();
        return none_object;
    }

    LZPY_IMP_METHOD_0(PyModel, getActionCount)
    {
        return build_object(model()->getActionCount());
    }

    LZPY_IMP_METHOD_1(PyModel, getActionName)
    {
        int idx;
        if (!parse_object(idx, value))
            return null_object;

        std::string name = model()->getActionName(idx);
        return build_object(name);
    }

    LZPY_IMP_METHOD_1(PyModel, setActionSpeed)
    {
        model()->setActionSpeed(parse_object<float>(value));
        return none_object;
    }
}