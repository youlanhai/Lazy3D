#pragma once


#define getXLParam(lp)   ((int)(short)LOWORD(lp))
#define getYLParam(lp)   ((int)(short)HIWORD(lp))

namespace Lazy
{

    class CPoint : public POINT
    {
    public:
        CPoint()
        {
            set(0, 0);
        }

        CPoint(const CPoint & pt)
        {
            set(pt.x, pt.y);
        }

        CPoint(const POINT * pt)
        {
            set(pt->x, pt->y);
        }

        CPoint(int x_, int y_)
        {
            set(x_, y_);
        }

        CPoint(LPARAM lParam)
        {
            formatLParam(lParam);
        }

        void set(int x_, int y_)
        {
            x = x_;
            y = y_;
        }

        void formatLParam(LPARAM lparam)
        {
            x = getXLParam(lparam);
            y = getYLParam(lparam);
        }

        LPARAM toLParam(void) const
        {
            return MAKELPARAM(x, y);
        }

        //运算符重载

        long & operator[](size_t i)
        {
            assert(i < 2);
            return ((long*) this)[i];
        }

        long operator[](size_t i) const
        {
            assert(i < 2);
            return ((long*) this)[i];
        }

        const CPoint & operator=(const CPoint & pt)
        {
            x = pt.x;
            y = pt.y;
            return (*this);
        }

        const CPoint & operator=(const POINT & pt)
        {
            x = pt.x;
            y = pt.y;
            return (*this);
        }

        bool operator==(const CPoint & pt) const
        {
            return (x == pt.x && y == pt.y);
        }

        bool operator==(const POINT & pt) const
        {
            return (x == pt.x && y == pt.y);
        }

        const CPoint & operator+=(const CPoint & pt)
        {
            x += pt.x;
            y += pt.y;
            return (*this);
        }

        const CPoint & operator-=(const CPoint & pt)
        {
            x -= pt.x;
            y -= pt.y;
            return (*this);
        }

        const CPoint & operator *= (int v)
        {
            x *= v;
            y *= v;
            return *this;
        }

        const CPoint & operator /= (int v)
        {
            x /= v;
            y /= v;
            return *this;
        }

        CPoint operator+(const CPoint & pt) const
        {
            return CPoint(x + pt.x, y + pt.y);
        }

        CPoint operator-(const CPoint & pt) const
        {
            return CPoint(x - pt.x, y - pt.y);
        }

        CPoint operator*(int v) const
        {
            return CPoint(x * v, y * v);
        }

        CPoint operator/(int v) const
        {
            return CPoint(x * v, y * v);
        }
    };


    class CSize : public SIZE
    {
    public:
        CSize(void)
        {
            cx = cy = 0;
        }
        CSize(int w, int h)
        {
            cx = w;
            cy = h;
        }
        CSize(const CSize & sz)
        {
            cx = sz.cx;
            cy = sz.cy;
        }
        CSize(CSize * psz)
        {
            cx = psz->cx;
            cy = psz->cy;
        }

        void set(int cx_, int cy_)
        {
            cx = cx_;
            cy = cy_;
        }

        const CSize & operator=(const CSize & sz)
        {
            set(sz.cx, sz.cy);
            return (*this);
        }

        const CSize & operator=(const SIZE & sz)
        {
            set(sz.cx, sz.cy);
            return (*this);
        }

        long & operator[](size_t i)
        {
            assert(i < 2);
            return ((long*)this)[i];
        }

        long  operator[](size_t i) const
        {
            assert(i < 2);
            return ((long*)this)[i];
        }

        bool operator==(const CSize & sz) const
        {
            return (cx == sz.cx && cy == sz.cy);
        }

        bool operator==(const SIZE & sz) const
        {
            return (cx == sz.cx && cy == sz.cy);
        }
    };

    class CRect : public RECT
    {
    public:
        CRect(void)
        {
            left = top = right = bottom = 0;
        };
        CRect(int left_, int top_, int right_, int bottom_)
        {
            set(left_, top_, right_, bottom_);
        }
        CRect(const CRect & rc)
        {
            *this = rc;
        }
        CRect(const RECT & rc)
        {
            *this = rc;
        }
        CRect(const CPoint & pt1, const CPoint & pt2)
        {
            set(pt1.x, pt1.y, pt2.x, pt2.y);
        }

        CRect(const CPoint & pt1, const CSize & size)
        {
            set(pt1.x, pt1.y, pt1.x + size.cx, pt1.y + size.cy);
        }

        void set(int left_, int top_, int right_, int bottom_)
        {
            left = left_;
            right = right_;
            top = top_;
            bottom = bottom_;
        }
        const CRect & operator=(const RECT & rc)
        {
            memcpy(this, &rc, sizeof(rc));
            return (*this);
        }

        const CRect & operator=(const CRect & rc)
        {
            memcpy(this, &rc, sizeof(rc));
            return (*this);
        }

        bool isEmpty() const
        {
            return left >= right || top >= bottom;
        }

        bool isIn(const CPoint & pt) const
        {
            return isIn(pt.x, pt.y);
        }

        bool isIn(const POINT *pt) const
        {
            return isIn(pt->x, pt->y);
        }

        bool isIn(int x, int y) const;
        bool isIntersect(const RECT * rc) const;

        ///获得当前区域与rc相交得到的区域
        void getIntersectRect(CRect & result, const CRect & rc) const;

        ///获得当前区域与rc合并得到的区域
        void getMergeRect(CRect & result, const CRect & rc) const;

        int width(void) const { return right - left; }
        void setWidth(int w);

        int height(void) const { return bottom - top; }
        void setHeight(int h);

        RECT toRect(void) const
        {
            RECT rc;
            rc.left = left;
            rc.right = right;
            rc.top = top;
            rc.bottom = bottom;
            return rc;
        }

        /** 偏移。*/
        void offset(int x, int y)
        {
            left += x;
            right += x;
            top += y;
            bottom += y;
        }

        void offset(const CPoint & pt)
        {
            offset(pt.x, pt.y);
        }

        /** 扩展或收缩*/
        void expand(int x, int y)
        {
            left -= x;
            right += x;
            top -= y;
            bottom += y;
        }
    };


    const CPoint PointZero;
    const CSize  SizeZero;
    const CRect  RectZero;

    ///表示一个纹理区域
    class UVRect
    {
    public:

        float left, top, right, bottom;

        UVRect()
        {}

        UVRect(float left_, float top_, float right_, float bottom_)
            : left(left_)
            , top(top_)
            , right(right_)
            , bottom(bottom_)
        {}

        UVRect(const CRect & rc);

        ~UVRect()
        {}

        void set(float left_, float top_, float right_, float bottom_);
        void setLeftTop(float left, float top);
        void setRightBottom(float right, float bottom);

        void makeZero();

        ///单位化成标准纹理坐标。(0,0,1,1)
        void makeIdentity();

        /** 偏移。*/
        void offset(int x, int y);

        /** 扩展或收缩*/
        void expand(int x, int y);

        float width() const { return right - left; }
        float height() const { return bottom - top; }

        const UVRect & operator=(const CRect & rc);
    };

    const UVRect IdentityUVRect(0.0f, 0.0f, 1.0f, 1.0f);

    std::ostream & operator << (std::ostream & out, const CPoint & v);
    std::ostream & operator << (std::ostream & out, const CSize & v);
    std::ostream & operator << (std::ostream & out, const CRect & v);
    std::ostream & operator << (std::ostream & out, const UVRect & v);


}//namespace Lazy