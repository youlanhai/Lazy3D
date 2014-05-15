#pragma once

#include <Windows.h>
#include <d3dx9math.h>

#include <cstdio>
#include <cassert>

#include <algorithm>
#include <map>
#include <list>
#include <vector>
#include <iostream>
#include <stdexcept>


///数学库命名空间
namespace Math
{
    const float MAX_FLOAT = 1e30f;
    const float MIN_FLOAT = -MAX_FLOAT;
    const float MIN_DIST = 0.001f;
    const float MIN_DIST_SQ = MIN_DIST * MIN_DIST;

    typedef std::vector<size_t>     IndicesArray;

    enum SIDE
    {
        SIDE_ON = 0,
        SIDE_FRONT = 1,
        SIDE_BACK = 2,
        SIDE_BOTH = 3,
        SIDE_FORCE_DWORD = 0x7fffffff
    };

    ///坐标轴索引
    enum AXIS_INDEX
    {
        AXIS_X = 0,
        AXIS_Y = 1,
        AXIS_Z = 2
    };


    inline float min2(float a, float b)
    {
        return a < b ? a : b;
    }

    inline float max2(float a, float b)
    {
        return a > b ? a : b;
    }

    inline bool amostZero(float d, float epsilon = 0.0004f)
    {
        return d > -epsilon && d < epsilon;
    }

}//end namespace Math