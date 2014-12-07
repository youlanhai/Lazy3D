#include "stdafx.h"
#include "LPyScript.h"

namespace Lzpy
{
    void exportLZD(const char *module);
    void exportPhysics(const char *module);

    LZPY_MODULE_BEG(Lazy)
        exportLZD(module);
        exportPhysics(module);
        LZPY_REGISTER_CLASS(SceneNode, PySceneNode);
    LZPY_MODULE_END()
}
