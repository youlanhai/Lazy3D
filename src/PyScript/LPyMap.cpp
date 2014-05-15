#include "stdafx.h"

#include "../Core/Core.h"
#include "LPyMap.h"
#include "LPyEntity.h"

namespace LazyPy
{

    LAZYPY_IMP("Map", PyMap, "Lazy");

    LAZYPY_BEGIN_EXTEN(PyMap);

        LAZYPY_GET(mapname);
        LAZYPY_GET(width);
        LAZYPY_GET(height);

        LAZYPY_GETSET(showLevel);
        LAZYPY_GETSET(showRadius);

        LAZYPY_METHOD_1(loadMap);
        LAZYPY_METHOD_1(saveMap);
        LAZYPY_METHOD_1(setSource);
        LAZYPY_METHOD_0(isUsefull);
        LAZYPY_METHOD_0(getBoundry);

    LAZYPY_END_EXTEN();

    PyMap::PyMap()
    {
        m_map = TerrainMap::instance();
    }

    PyMap::~PyMap()
    {
        m_map = nullptr;
    }

    LAZYPY_IMP_INIT(PyMap)
    {
        return true;
    }

    LAZYPY_IMP_GET(PyMap, mapname)
    {
        return toPyObject(m_map->getMapName());
    }

    LAZYPY_IMP_GET(PyMap, width)
    {
        return toPyObject(m_map->width());
    }
    LAZYPY_IMP_GET(PyMap, height)
    {
        return toPyObject(m_map->height());
    }

    LAZYPY_IMP_GET(PyMap, showLevel)
    {
        return toPyObject(m_map->getShowLevel());
    }

    LAZYPY_IMP_SET(PyMap, showLevel)
    {
        m_map->setShowLevel(fromPyObject<bool>(value));
    }


    LAZYPY_IMP_GET(PyMap, showRadius)
    {
        return toPyObject(m_map->getShowRadius());
    }

    LAZYPY_IMP_SET(PyMap, showRadius)
    {
        m_map->setShowRadius(fromPyObject<bool>(value));
    }

    LAZYPY_IMP_METHOD_1(PyMap, loadMap)
    {
        bool ret = m_map->loadMap(fromPyObject<std::wstring>(value));
        return toPyObject(ret);
    }

    LAZYPY_IMP_METHOD_1(PyMap, saveMap)
    {
        m_map->saveMap(fromPyObject<std::wstring>(value));
        Py_RETURN_NONE;
    }

    LAZYPY_IMP_METHOD_1(PyMap, setSource)
    {
        if (value != Py_None && !PyEntity_Check(value)) return NULL;

        iEntity *ent = nullptr;
        if (value)
        {
            ent = ((PyEntity*) value)->m_entity.get();
        }

        m_map->setSource(ent);
        Py_RETURN_NONE;
    }

    LAZYPY_IMP_METHOD_0(PyMap, isUsefull)
    {
        return toPyObject(m_map->isUserfull());
    }

    LAZYPY_IMP_METHOD_0(PyMap, getBoundry)
    {
        return Py_BuildValue("ffff", m_map->xMin(),
            m_map->zMin(), m_map->xMax(), m_map->zMax());
    }
 
    ////////////////////////////////////////////////////////////////////
    static PyMap * s_pMap = nullptr;


    LAZYPY_DEF_FUN_0(map)
    {
        return toPyObject(s_pMap);
    }

    LAZYPY_DEF_FUN_1(setMap)
    {
        if (s_pMap == value) Py_RETURN_NONE;

        if (value != Py_None && !helper::has_instance<PyMap>(value, true)) return nullptr;
        
        Py_DECREF(s_pMap);
        s_pMap = (PyMap*)value;
        Py_INCREF(s_pMap);

        Py_RETURN_NONE;
    }

    namespace _py_map
    {

        class ResLoader : public LazyPyResInterface
        {
        public:
            void init() override
            {
                s_pMap = helper::new_instance_ex<PyMap>();
            }

            void fini() override
            {
                Py_DECREF(s_pMap);
            }
        };

        static ResLoader s_resLoader;


        LAZYPY_BEGIN_FUN(Lazy);

        LAZYPY_FUN_0(map);
        LAZYPY_FUN_1(setMap);

        LAZYPY_END_FUN();

    }
}
