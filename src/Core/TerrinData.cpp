

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

} // end namespace Lazy
