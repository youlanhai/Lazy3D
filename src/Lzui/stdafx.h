﻿// stdafx.h : 标准系统包含文件的包含文件，
// 或是经常使用但不常更改的
// 特定于项目的包含文件
//

#pragma once

#define WIN32_LEAN_AND_MEAN             // 从 Windows 头中排除极少使用的资料


// TODO: 在此处引用程序需要的其他头文件

#include "UIConfig.h"

#include "../utility/Utility.h"
#include "../Math/Math.h"

#ifdef ENABLE_SCRIPT
#include "../Lzpy/Lzpy.h"
#endif
