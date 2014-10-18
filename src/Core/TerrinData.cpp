

#include "stdafx.h"
#include "TerrinData.h"
#include "../utility/Utility.h"

namespace Lazy
{

    namespace MapConfig
    {

        bool ShowChunkOctree = false;

        bool CanSelectItem = false;

        bool ShowItemOctree = false;

        bool ShowAllItemAABB = false;

        bool UseMultiThread = false;

        bool ShowTerrain = true;
    }


    DWORD TerrinVertex::FVF = D3DFVF_XYZ | D3DFVF_NORMAL | D3DFVF_TEX2; //<灵活顶点格式
    int TerrinVertex::SIZE = sizeof(TerrinVertex);

    HeightMap::HeightMap()
        : m_rows(0)
        , m_cols(0)
        , m_gridSize(0.0f)
    {}

    bool HeightMap::load(const tstring & filename)
    {
        m_resource = filename;
        LZDataPtr root = openSection(filename, false, DataType::Lzd);
        if (!root)
        {
            LOG_ERROR(L"Failed to load height map config '%s'", filename.c_str());
            return false;
        }

        m_rows = root->readInt(L"rows");
        m_cols = root->readInt(L"cols");
        m_gridSize = root->readFloat(L"gridSize", 1.0f);
        
        float heightScale = root->readFloat(L"heightScale", 1.0f);

    }

    float HeightMap::getHeight(float x, float z) const
    {

    }

} // end namespace Lazy
