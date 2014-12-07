#include "stdafx.h"
#include "LPyScript.h"

namespace Lzpy
{
    void exportLZD(const char *module);
    void exportPhysics(const char *module);
    void exportEntity(const char * module);
    void exportMap(const char * module);
    void exportMisc(const char * module);

    LZPY_MODULE_BEG(Lazy)
        exportLZD(module);
        exportPhysics(module);
        LZPY_REGISTER_CLASS(SceneNode, PySceneNode);
        exportEntity(module);
        exportMap(module);
        exportMisc(module);
    LZPY_MODULE_END()
}
