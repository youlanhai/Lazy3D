#pragma once

namespace Lzpy
{
    using namespace Lazy;

    class PyMap : public PyBase
    {
        LZPY_DEF(PyMap, PyBase);
    public:
        PyMap();
        ~PyMap();

        LZPY_DEF_GET(name, m_map->getMapName);
        LZPY_DEF_GET(width, m_map->width);

        LZPY_DEF_GET(height, m_map->height);

        LZPY_DEF_GET(showLevel, m_map->getShowLevel);
        LZPY_DEF_SET(showLevel, m_map->setShowLevel, int);

        LZPY_DEF_GET(showRadius, m_map->getShowRadius);
        LZPY_DEF_SET(showRadius, m_map->setShowRadius, float);

        LZPY_DEF_GET(isUsefull, m_map->isUserfull);
        LZPY_DEF_GET(boundry, getBoundry);

        LZPY_DEF_METHOD_1(loadMap);
        LZPY_DEF_METHOD_1(saveMap);

        LZPY_DEF_METHOD_1(setSource);

        object getBoundry();

        TerrainMap *m_map;
    };

    void exportPyMap(const char * module);
}

