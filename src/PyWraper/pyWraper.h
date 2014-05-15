#pragma once

#include "Header.h"

#if PY_VERSION_HEX >= 0x03000000
#   define PY_INIT_FUN(name) PyObject * PyInit_##name()
#else
#   define PY_INIT_FUN(name) void init#name()
#endif

extern "C" __declspec(dllexport) PY_INIT_FUN(Lazy);
