#include "stdafx.h"
#include "LPyScript.h"
#include "LPyPoint.h"

namespace Lazy
{
    void exportLZD(const char *module);
    void exportPhysics(const char *module);
    void exportEntity(const char * module);
    void exportMap(const char * module);
    void exportMisc(const char * module);
    void exportUI(const char *module);

    LZPY_MODULE_BEG(Lazy)
        exportLZD(module);
        exportPhysics(module);
        LZPY_REGISTER_CLASS(SceneNode, PySceneNode);
        LZPY_REGISTER_CLASS(Point, PyPoint);
        LZPY_REGISTER_CLASS(Rect, PyRect);
        exportEntity(module);
        exportMap(module);
        exportMisc(module);
        exportUI(module);
    LZPY_MODULE_END()
}
