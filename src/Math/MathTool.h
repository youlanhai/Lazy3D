#pragma once

#include "../utility/DataSection/lzd.h"

namespace Lazy
{
    class Vector2;
    class Vector3;
    class Matrix;

    bool readVector2(LZDataPtr ptr, const tstring & tag, Vector2 & vec);
    bool writeVector2(LZDataPtr ptr, const tstring & tag, const Vector2 & vec);

    bool readVector3(LZDataPtr ptr, const tstring & tag, Vector3 & vec);
    bool writeVector3(LZDataPtr ptr, const tstring & tag, const Vector3 & vec);

    bool readMatrix(LZDataPtr ptr, const tstring & tag, Matrix & mat);
    bool writeMatrix(LZDataPtr ptr, const tstring & tag, const Matrix & mat);

}