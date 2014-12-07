#include "stdafx.h"

#include "../Core/Core.h"
#include "LPyMap.h"
#include "LPyEntity.h"

namespace Lzpy
{

    LZPY_CLASS_EXPORT(PyMap)
    {

        LZPY_GET(name);
        LZPY_GET(isUsefull);
        LZPY_GET(rect);

        LZPY_GETSET(showRadius);
        LZPY_GETSET(source);

        LZPY_METHOD_1(loadMap);
        LZPY_METHOD_1(saveMap);
        LZPY_METHOD(createMap);
    }

    PyMap::PyMap()
    {
        m_map = TerrainMap::instance();
    }

    PyMap::~PyMap()
    {
        m_map = nullptr;
    }

    LZPY_IMP_INIT(PyMap)
    {
        PyErr_SetString(PyExc_RuntimeError, "PyMap can't create instance.");
        return false;
    }

    LZPY_IMP_METHOD_1(PyMap, loadMap)
    {
        Lazy::tstring name;
        if (!parse_object(name, value))
            return null_object;

        bool ret = m_map->loadMap(name);
        return build_object(ret);
    }

    LZPY_IMP_METHOD_1(PyMap, saveMap)
    {
        Lazy::tstring name;
        if (!parse_object(name, value))
            return null_object;

        m_map->saveMap(name);
        return none_object;
    }

    LZPY_IMP_METHOD(PyMap, createMap)
    {
        tstring name;
        int row, col;
        if (!arg.parse_tuple(&name, &row, &col))
            return null_object;
        
        return build_object(m_map->createMap(name, row, col));
    }

    ////////////////////////////////////////////////////////////////////
    static PyMap * s_pMap = nullptr;


    LZPY_DEF_FUN_0(map)
    {
        return xincref(s_pMap);
    }

    namespace _py_map
    {

        class ResLoader : public LzpyResInterface
        {
        public:
            void init() override
            {
                s_pMap = new_instance_ex<PyMap>();
            }

            void fini() override
            {
                Py_DECREF(s_pMap);
            }
        };

        static ResLoader s_resLoader;
    }

    void exportMap(const char * module)
    {
        LZPY_REGISTER_CLASS(Map, PyMap);

        LZPY_FUN_0(map);
    }
}
