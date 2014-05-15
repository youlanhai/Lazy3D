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
#include "../Math/Math.h"

class CApp;
LZDLL_API CApp*  getApp(void);


//以下内容为兼容旧接口

using Lazy::IBase;
using Lazy::RefPtr;
using Lazy::CRect;
using Lazy::CPoint;
using Lazy::CSize;

using namespace Math;

#endif //LAZY3D_BASE_H