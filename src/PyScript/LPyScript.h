#pragma once

#ifndef LAZPY_SCRIPT_H
#define LAZPY_SCRIPT_H

#include "LPyPhysics.h"
#include "LPySceneNode.h"
#include "LPyEntity.h"
#include "LPyMisc.h"
#include "LPyMap.h"

#include "LPyLzd.h"
#include "LPyUIMisc.h"
#include "LPyConsole.h"

namespace Lzpy
{
    ///导出模块方法
    LZPY_DEF_MODULE(Lazy);
}

#endif //LAZPY_SCRIPT_H