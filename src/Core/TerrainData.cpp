

#include "stdafx.h"
#include "TerrainData.h"
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

    template<typename T>
    static void parseHeightMap(std::vector<float> & output, 
        int rows, int cols, float scale, const T * input)
    {
        T val;
        for (int r = 0; r < rows; ++r)
        {
            for (int c = 0; c < cols; ++c)
            {
                val = *(input++);
                output.push_back(val * scale);
            }
        }
    }

    DWORD TerrinVertex::FVF = D3DFVF_XYZ | D3DFVF_NORMAL | D3DFVF_TEX2; //<灵活顶点格式
    int TerrinVertex::SIZE = sizeof(TerrinVertex);

    HeightMap::HeightMap()
        : m_rows(0)
        , m_cols(0)
        , m_gridSize(0.0f)
        , m_origin(MathConst::vec3Zero)
        , m_diagonalDistanceX4(0.0f)
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
        readVector3(root, L"origin", m_origin);

        // sqrt( x * x + z * z)
        m_diagonalDistanceX4 = sqrt(m_gridSize * m_gridSize * 2.0f) * 4.0f;

        tstring rawfile = root->readString(L"rawFile");
        int depth = root->readInt(L"rawBit");
        if (depth != 8 && depth != 16 && depth != 32)
        {
            LOG_ERROR(L"Unsupported raw bit '%d', must be 8, 16 or 32", depth);
            return false;
        }

        float heightScale = root->readFloat(L"heightScale", 1.0f);

        std::vector<char> rawdata;
        if (!getfs()->readBinary(rawfile, rawdata))
        {
            LOG_ERROR(L"Failed to load raw file '%s'", rawfile.c_str());
            return false;
        }

        size_t needSize = m_rows * m_cols * depth >> 3;
        if (rawdata.size() < needSize)
        {
            LOG_ERROR(L"Invalid raw file '%s', data is too smal.", rawfile.c_str());
            return false;
        }

        m_rawdata.clear();

        if (depth == 8)
            parseHeightMap<uint8>(m_rawdata, m_rows, m_cols, heightScale, (uint8*) rawdata.data());
        else if (depth == 16)
            parseHeightMap<uint16>(m_rawdata, m_rows, m_cols, heightScale, (uint16*) rawdata.data());
        else if (depth == 32)
            parseHeightMap<uint32>(m_rawdata, m_rows, m_cols, heightScale, (uint32*) rawdata.data());
        else
            assert(0);

        return true;
    }

    float HeightMap::getAbsHeight(int col, int row) const
    {
        if (row >= m_rows || row < 0 || col >= m_cols || col < 0)
            return 0.0f;

        return m_rawdata[row * m_cols + col];
    }

    float HeightMap::getHeight(float x, float z) const
    {
        // calculate the x and z positions on the height map.
        float xs = (x - m_origin.x) / m_gridSize;
        float zs = (z - m_origin.z) / m_gridSize;

        // calculate the fractional coverage for the x/z positions
        float xf = xs - floorf(xs);
        float zf = zs - floorf(zs);

        // find the height map start cell of this height
        int32 xOff = int32(floorf(xs));
        int32 zOff = int32(floorf(zs));

        float ret = 0;

        // the cells diagonal goes from top left to bottom right.
        // Get the heights for the diagonal
        float h00 = getAbsHeight(xOff, zOff);
        float h11 = getAbsHeight(xOff + 1, zOff + 1);

        /*
        *  h00--h10
        *   |\   |
        *   | \  |
        *   |  \ |
        *  h01--h11
        */

        // Work out which triangle we are in and calculate the interpolated
        // height.
        if (xf > zf)
        {
            float h10 = getAbsHeight(xOff + 1, zOff);
            ret = h10 + (h00 - h10) * (1.f - xf) + (h11 - h10) * zf;
        }
        else
        {
            float h01 = getAbsHeight(xOff, zOff + 1);
            ret = h01 + (h11 - h01) * xf + (h00 - h01) * (1.f - zf);
        }

        return ret;
    }


    Vector3 HeightMap::getNormal(int x, int z) const
    {
       const float diagonalMultiplier = 0.7071067811f;// sqrt( 0.5 )

        Vector3 ret;
        ret.y = m_diagonalDistanceX4 + (m_gridSize * 2 + m_gridSize * 2);
        ret.x = getAbsHeight(x - 1, z) - getAbsHeight(x + 1, z);
        ret.z = getAbsHeight(x, z - 1) - getAbsHeight(x, z + 1);

        float val = getAbsHeight(x - 1, z - 1) - getAbsHeight(x + 1, z + 1);
        val *= diagonalMultiplier;

        ret.x += val;
        ret.z += val;

        val = getAbsHeight(x - 1, z + 1) - getAbsHeight(x + 1, z - 1);
        val *= diagonalMultiplier;

        ret.x += val;
        ret.z -= val;
        ret.normalize();

        return ret;
    }


    Vector3  HeightMap::getNormal(float x, float z) const
    {
        float xf = (x - m_origin.x) / m_gridSize;
        float zf = (z - m_origin.z) / m_gridSize;

        Vector3 ret;
        float xFrac = xf - ::floorf(xf);
        float zFrac = zf - ::floorf(zf);

        xf -= xFrac;
        zf -= zFrac;

        int xfi = (int) xf;
        int zfi = (int) zf;
        Vector3 n1 = getNormal(xfi, zfi);
        Vector3 n2 = getNormal(xfi + 1, zfi);
        Vector3 n3 = getNormal(xfi, zfi + 1);
        Vector3 n4 = getNormal(xfi + 1, zfi + 1);

        n1 *= (1 - xFrac)*(1 - zFrac);
        n2 *= (xFrac) *(1 - zFrac);
        n3 *= (1 - xFrac)*(zFrac);
        n4 *= (xFrac) *(zFrac);

        ret = n1;
        ret += n2;
        ret += n3;
        ret += n4;

        ret.normalize();
        return ret;
    }

} // end namespace Lazy
