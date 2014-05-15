
#include "stdafx.h"
#include "Tools2d.h"

namespace Lazy
{

    ////////////////////////////////////////////////////////////////////
    void CRect::setWidth(int w)
    {
        right = left + w;
    }

    void CRect::setHeight(int h)
    {
        bottom = top + h;
    }

    bool CRect::isIn(int x, int y) const
    {
        if (x >= left && x <= right && y >= top && y <= bottom)
        {
            return true;
        }
        else return false;
    }
    bool CRect::isIntersect(const RECT * rc) const
    {
        if (right<rc->left || left>rc->right
            || bottom<rc->top || top>rc->bottom)
        {
            return false;
        }
        else return true;
    }

    void CRect::getIntersectRect(CRect & result, const CRect & rc) const
    {
        result.left = max(left, rc.left);
        result.top = max(top, rc.top);
        result.right = min(right, rc.right);
        result.bottom = min(bottom, rc.bottom);
    }

    void CRect::getMergeRect(CRect & result, const CRect & rc) const
    {
        result.left = min(left, rc.left);
        result.top = min(top, rc.top);
        result.right = max(right, rc.right);
        result.bottom = max(bottom, rc.bottom);
    }

    ////////////////////////////////////////////////////////////////////

    UVRect::UVRect(const CRect & rc)
    {
        left = float(rc.left);
        top = float(rc.top);
        right = float(rc.right);
        bottom = float(rc.bottom);
    }

    const UVRect & UVRect::operator=(const CRect & rc)
    {
        left = float(rc.left);
        top = float(rc.top);
        right = float(rc.right);
        bottom = float(rc.bottom);

        return *this;
    }

    void UVRect::set(float left_, float top_, float right_, float bottom_)
    {
        left = left_;
        top = top_;
        right = right_;
        bottom = bottom_;
    }

    void UVRect::setLeftTop(float left_, float top_)
    {
        left = left_;
        top = top_;
    }

    void UVRect::setRightBottom(float right_, float bottom_)
    {
        right = right_;
        bottom = bottom_;
    }

    void UVRect::offset(int x, int y)
    {
        left += x;
        right += x;
        top += y;
        bottom += y;
    }

    void UVRect::expand(int x, int y)
    {
        left -= x;
        right += x;
        top -= y;
        bottom += y;
    }

    void UVRect::makeZero()
    {
        left = top = right = bottom = 0.0f;
    }

    void UVRect::makeIdentity()
    {
        left = top = 0.0f;
        right = bottom = 1.0f;
    }

    ////////////////////////////////////////////////////////////////////
    std::ostream & operator << (std::ostream & out, const CPoint & v)
    {
        out << "(" << v.x << "," << v.y << ")";
        return out;
    }

    std::ostream & operator << (std::ostream & out, const CSize & v)
    {
        out << "(" << v.cx << "," << v.cy << ")";
        return out;
    }

    std::ostream & operator << (std::ostream & out, const CRect & v)
    {
        out << "(" << v.left << "," << v.top << "," << v.right << "," << v.bottom << ")";
        return out;
    }

    std::ostream & operator << (std::ostream & out, const UVRect & v)
    {
        out << "(" << v.left << "," << v.top << "," << v.right << "," << v.bottom << ")";
        return out;
    }
}