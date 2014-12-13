#pragma once

#include "../Core/Model.h"
#include "LPySceneNode.h"

namespace Lzpy
{

    class PyModel : public PySceneNode
    {
        LZPY_DEF(PyModel);
    public:
        PyModel();

        inline IModel * model() { return (IModel*)m_object; }

        LZPY_DEF_GET(resource, model()->source);
        LZPY_DEF_GET(height, model()->getHeight);

        LZPY_DEF_GET(aabbVisible, model()->getBBVisible);
        LZPY_DEF_SET(aabbVisible, model()->setBBVisible, bool);

        LZPY_DEF_METHOD(playAction);
        LZPY_DEF_METHOD_0(stopAction);
        LZPY_DEF_METHOD_0(getActionCount);
        LZPY_DEF_METHOD_1(getActionName);
        LZPY_DEF_METHOD_1(setActionSpeed);
    };

    BUILD_AND_PARSE_SCRIPT_OBJECT(PyModel, IModel);


}