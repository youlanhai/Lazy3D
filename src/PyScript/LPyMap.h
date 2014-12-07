#pragma once

#include "LPySceneNode.h"

namespace Lzpy
{
    using namespace Lazy;

    class PyMap : public PyBase
    {
        LZPY_DEF(PyMap);
    public:
        PyMap();
        ~PyMap();

        LZPY_DEF_GET(name, m_map->getMapName);

        LZPY_DEF_GET(showRadius, m_map->getShowRadius);
        LZPY_DEF_SET(showRadius, m_map->setShowRadius, float);

        LZPY_DEF_GET(isUsefull, m_map->isUserfull);
        LZPY_DEF_GET(rect, m_map->getRect);

        LZPY_DEF_GET(source, m_map->getSource);
        LZPY_DEF_SET(source, m_map->setSource, SceneNode*);

        LZPY_DEF_METHOD_1(loadMap);
        LZPY_DEF_METHOD_1(saveMap);
        LZPY_DEF_METHOD(createMap);

        TerrainMap *m_map;
    };
}

