//Commen.h
#pragma once

#ifndef LZ3DENGINE_COMMEN_H
#define LZ3DENGINE_COMMEN_H

#include <windows.h>
#include <mmsystem.h>
#include <tchar.h>

#include <d3d9.h>
#include <d3dx9.h>
#include <dsound.h>

#include <cstdlib>
#include <cstdarg>
#include <cstdio>
#include <cmath>
#include <cassert>
#include <string>
#include <list>
#include <vector>
#include <map>
#include <set>
#include <algorithm>
#include <typeinfo>

#define KEYDOWN(vk_code) ((GetAsyncKeyState(vk_code) & 0x8000) ? 1 : 0)
#define KEYUP(vk_code) ((GetAsyncKeyState(vk_code) & 0x8000) ? 0 : 1)
#define ISKEYDOWN(keyboard, key) (keyboard[key] & 0x80)
#define ISKEYUP(keyboard, key)	(!(keyboard[key] & 0x80))

#define NOT_IMPLEMENT assert(NULL && "method not implement!")

#include "Base.h"

#endif //LZ3DENGINE_COMMEN_H