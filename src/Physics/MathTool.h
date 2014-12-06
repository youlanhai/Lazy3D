#pragma once

#include "../utility/DataSection/lzd.h"

namespace Lazy
{
    class Vector2;
    class Vector3;
    class Vector4;
    class Quaternion;
    class Matrix;
    class AABB;

    bool readVector2(LZDataPtr ptr, const tstring & tag, Vector2 & vec);
    void writeVector2(LZDataPtr ptr, const tstring & tag, const Vector2 & vec);

    bool readVector3(LZDataPtr ptr, const tstring & tag, Vector3 & vec);
    void writeVector3(LZDataPtr ptr, const tstring & tag, const Vector3 & vec);

    bool readVector4(LZDataPtr ptr, const tstring & tag, Vector4 & vec);
    void writeVector4(LZDataPtr ptr, const tstring & tag, const Vector4 & vec);

    bool readQuaternion(LZDataPtr ptr, const tstring & tag, Quaternion & vec);
    void writeQuaternion(LZDataPtr ptr, const tstring & tag, const Quaternion & vec);

    bool readMatrix(LZDataPtr ptr, const tstring & tag, Matrix & mat);
    void writeMatrix(LZDataPtr ptr, const tstring & tag, const Matrix & mat);

    bool readAABB(LZDataPtr ptr, const tstring & tag, AABB & mat);
    void writeAABB(LZDataPtr ptr, const tstring & tag, const AABB & mat);
}