#include "StdAfx.h"
#include "LzpyEmbed.h"

namespace Lzpy
{
    object import(const std::wstring & name)
    {
        return new_reference( PyImport_Import( str(name).get() ) );
    }

    object import(const char *name)
    {
        return new_reference(PyImport_ImportModule(name));
    }

    void addSysPath(const std::wstring & path)
    {
        std::vector<std::wstring> paths;
        paths.push_back(path);
        addSysPaths(paths);
    }

    void addSysPaths(const std::vector<std::wstring> & paths)
    {
        object sys = import("sys");
        list syspath = sys.getattr("path");

        for (const std::wstring & path : paths)
            syspath.append(path);
    }
}