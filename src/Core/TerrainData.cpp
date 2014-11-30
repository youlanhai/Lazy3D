

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

    template<typename T>
    static void parseHeightMap(std::vector<float> & output, 
        int rows, int cols, float scale, const std::vector<char> & input)
    {
        T val;
        size_t i = 0;
        for (int r = 0; r < rows; ++r)
        {
            for (int c = 0; c < cols; ++c)
            {
                val = *((T*) &input[i++]);
                output.push_back(val * scale);
            }
        }
    }

    static float lerp(float a, float b, float t)
    {
        return a - (a * t) + (b * t);
    }

    DWORD TerrinVertex::FVF = D3DFVF_XYZ | D3DFVF_NORMAL | D3DFVF_TEX2; //<灵活顶点格式
    int TerrinVertex::SIZE = sizeof(TerrinVertex);

    HeightMap::HeightMap()
        : m_rows(0)
        , m_cols(0)
        , m_gridSize(0.0f)
        , m_origin(MathConst::vec3Zero)
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

        if (depth == 16)
            parseHeightMap<uint16>(m_rawdata, m_rows, m_cols, heightScale, rawdata);
        else if (depth == 32)
            parseHeightMap<uint32>(m_rawdata, m_rows, m_cols, heightScale, rawdata);
        else
            parseHeightMap<uint8> (m_rawdata, m_rows, m_cols, heightScale, rawdata);
        
        return true;
    }

    float HeightMap::getAbsHeight(int row, int col) const
    {
        if (row >= m_rows) row = m_rows - 1;
        else if (row < 0) row = 0;

        if (col >= m_cols) col = m_cols - 1;
        else if (col < 0) col = 0;

        return m_rawdata[row * m_cols + col];
    }

    float HeightMap::getHeight(float x, float z) const
    {
        x = (x - m_origin.x) / m_gridSize;
        z = (z - m_origin.z) / m_gridSize;

        //计算x,z坐标所在的行列值
        int col = int(x);//向下取整
        int row = int(z);

        // 获取如下图4个顶点的高度
        //
        //  A   B
        //  *---*
        //  | / |
        //  *---*
        //  C   D

        float A = getAbsHeight(row, col);
        float B = getAbsHeight(row, col + 1);
        float C = getAbsHeight(row + 1, col);
        float D = getAbsHeight(row + 1, col + 1);

        float height;
#if 1
        height = (A + B + C + D) * 0.25f;
#else
        float dx = x - col;
        float dz = z - row;

        if (dz < 1.0f - dx)//(x,z)点在ABC三角形上
        {
            float uy = B - A;
            float vy = C - A;

            height = A + lerp(0.0f, uy, dx) + lerp(0.0f, vy, dz);//线形插值得到高度
        }
        else//(x,z)点在BCD三角形上
        {
            float uy = C - D;
            float vy = B - D;

            height = D + lerp(0.0f, uy, 1.0f - dx) + lerp(0.0f, vy, 1.0f - dz);
        }
#endif
        return height + m_origin.y;
    }

} // end namespace Lazy
