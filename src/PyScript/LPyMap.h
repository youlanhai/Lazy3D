#pragma once

namespace LazyPy
{


    class PyMap : public PyBase
    {
        LAZYPY_DEF(PyMap, PyBase);
    public:
        PyMap();
        ~PyMap();

        LAZYPY_DEF_GET(mapname);
        LAZYPY_DEF_GET(width);
        LAZYPY_DEF_GET(height);

        LAZYPY_DEF_GETSET(showLevel);
        LAZYPY_DEF_GETSET(showRadius);

        LAZYPY_DEF_METHOD_1(loadMap);
        LAZYPY_DEF_METHOD_1(saveMap);
        LAZYPY_DEF_METHOD_1(setSource);
        LAZYPY_DEF_METHOD_0(isUsefull);
        LAZYPY_DEF_METHOD_0(getBoundry);

        TerrainMap *m_map;
    };
}

