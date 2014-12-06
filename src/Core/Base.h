#pragma once

#ifndef LAZY3D_BASE_H
#define LAZY3D_BASE_H

#ifndef LZ3DENGINE_DYNAMIC_LINK
#   define LZDLL_API
#else
#   ifdef LZ3DENGINE_EXPORTS
#       define LZDLL_API __declspec(dllexport)
#   else
#       define LZDLL_API __declspec(dllimport)
#   endif
#endif

#include "../utility/Utility.h"
#include "../Render/Config.h"
#include "../Physics/Math.h"

namespace Lazy
{
    class CApp;
    LZDLL_API CApp*  getApp(void);

}// end namespace Lazy

#endif //LAZY3D_BASE_H