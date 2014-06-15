#pragma once

#ifndef LAZY_PY_WRAPPER_H
#define LAZY_PY_WRAPPER_H

#include "LzpyObject.h"
#include "LzpyTuple.h"
#include "LzpyStr.h"
#include "LzpyList.h"
#include "LzpyDict.h"

namespace Lzpy
{
    object import(const std::wstring & str);
    object import(const char *name);
    void addSysPath(const std::wstring & path);
    void addSysPaths(const std::vector<std::wstring> & paths);
}

#endif //LAZY_PY_WRAPPER_H
