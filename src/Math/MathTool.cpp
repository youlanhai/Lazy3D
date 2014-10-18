#include "stdafx.h"
#include "MathTool.h"

#include "Vector.h"
#include "Matrix.h"

#include <sstream>

namespace Lazy
{
    bool readVector2(LZDataPtr ptr, const tstring & tag, Vector2 & vec)
    {
        const tstring & val = ptr->readString(tag);
        if (val.empty()) return false;

        std::wistringstream ss(val);
        ss >> vec.x >> vec.y;
        return true;
    }

    void writeVector2(LZDataPtr ptr, const tstring & tag, const Vector2 & vec)
    {
        std::wostringstream ss;
        ss << vec.x << ' ' << vec.y;
        ptr->writeString(tag, ss.str());
    }

    bool readVector3(LZDataPtr ptr, const tstring & tag, Vector3 & vec)
    {
        const tstring & val = ptr->readString(tag);
        if (val.empty()) return false;

        std::wistringstream ss(val);
        ss >> vec.x >> vec.y >> vec.z;
        return true;
    }

    void writeVector3(LZDataPtr ptr, const tstring & tag, const Vector3 & vec)
    {
        std::wostringstream ss;
        ss << vec.x << ' ' << vec.y << ' ' << vec.z;
        ptr->writeString(tag, ss.str());
    }


    bool readVector4(LZDataPtr ptr, const tstring & tag, Vector4 & vec)
    {
        const tstring & val = ptr->readString(tag);
        if (val.empty()) return false;

        std::wistringstream ss(val);
        ss >> vec.x >> vec.y >> vec.z >> vec.w;
        return true;
    }

    void writeVector4(LZDataPtr ptr, const tstring & tag, const Vector4 & vec)
    {
        std::wostringstream ss;
        ss << vec.x << ' ' << vec.y << ' ' << vec.z << ' ' << vec.w;
        ptr->writeString(tag, ss.str());
    }

    bool readMatrix(LZDataPtr ptr, const tstring & tag, Matrix & mat)
    {
        const tstring & val = ptr->readString(tag);
        if (val.empty()) return false;

        std::wistringstream ss(val);
        for (int i = 0; i < 16; ++i)
            ss >> ((float*)(mat))[i];

        return true;
    }

    void writeMatrix(LZDataPtr ptr, const tstring & tag, const Matrix & mat)
    {
        std::wostringstream ss;

        for (int i = 0; i < 16; ++i)
            ss << ((const float*) (mat))[i] << ' ';

        ptr->writeString(tag, ss.str());
    }

}