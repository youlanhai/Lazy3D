#include "stdafx.h"
#include "MathTool.h"

#include "Vector.h"
#include "Matrix.h"

#include "AABB.h"

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

    bool readQuaternion(LZDataPtr ptr, const tstring & tag, Quaternion & vec)
    {
        return readVector4(ptr, tag, *((Vector4*) &vec));
    }

    void writeQuaternion(LZDataPtr ptr, const tstring & tag, const Quaternion & vec)
    {
        writeVector4(ptr, tag, *((const Vector4*) &vec));
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


    bool readAABB(LZDataPtr ptr, const tstring & tag, AABB & ab)
    {
        const tstring & val = ptr->readString(tag);
        if (val.empty()) return false;

        std::wistringstream ss(val);
        ss >> ab.min.x >> ab.min.y >> ab.min.z
            >> ab.max.x >> ab.max.y >> ab.max.z;

        return true;
    }

    void writeAABB(LZDataPtr ptr, const tstring & tag, const AABB & ab)
    {
        std::wostringstream ss;

        ss << ab.min.x << ' '
            << ab.min.y << ' '
            << ab.min.z << ' '
            << ab.max.x << ' '
            << ab.max.y << ' '
            << ab.max.z;

        ptr->writeString(tag, ss.str());
    }
}