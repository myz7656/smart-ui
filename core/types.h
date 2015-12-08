#ifndef __core_types_h__
#define __core_types_h__

#include <windows.h>
#include <gdiplus.h>

namespace core
{
    typedef Gdiplus::RectF RECTF;

    inline RECT RECTF2RECT(RECTF& rect_f)
    {
        RECT rect;
        rect.left = (int)(rect_f.GetLeft());
        rect.top = (int)(rect_f.GetTop());
        rect.right = (int)(rect_f.GetRight() + 0.9);
        rect.bottom = (int)(rect_f.GetBottom() + 0.9);

        return rect;
    }

    inline RECTF RECT2RECTF(RECT& rect)
    {
        return RECTF((float)rect.left, (float)rect.top, (float)(rect.right - rect.left), (float)(rect.bottom - rect.top));
    }

    struct Color
    {
        union
        {
            unsigned long value;
            struct rgb
            {
                byte r;
                byte g;
                byte b;
                byte a;
            } rgb_value;
        };

        Color() : value(0) {}

        Color(unsigned long value) : value(value) {}

        operator unsigned long() { return value; }
    };



    struct Unit
    {
        int use;
        bool pixel;

        Unit() 
            : use(0)
            , pixel(true) {}

        Unit(int u)
            : use(u)
            , pixel(true) {}

        Unit(int u, bool p)
            : use(u)
            , pixel(p) {}

        int operator()(int length) const
        {
            if (!pixel)
            {
                return use * length / 100;
            }
            return use;
        }

        Unit & operator=(int u)
        {
            use = u;
            pixel = true;
            return *this;
        }
    };



    struct BoxParam
    {
        int left;
        int top;
        int right;
        int bottom;

        int margin_left;
        int margin_top;
        int margin_right;
        int margin_bottom;

        int border;

        int padding_left;
        int padding_top;
        int padding_right;
        int padding_bottom;

        int width;
        int height;

        BoxParam()
            : left(0)
            , top(0)
            , right(0)
            , bottom(0)
            , margin_left(0)
            , margin_top(0)
            , margin_right(0)
            , margin_bottom(0)
            , border(0)
            , padding_left(0)
            , padding_top(0)
            , padding_right(0)
            , padding_bottom(0)
            , width(0)
            , height(0) {}
    };



    struct LayoutParam
    {
        bool leaf;
        RECT content;
        int  row;
        int  col;
        int  baseline;
        RECT boundary;

        LayoutParam()
            : leaf(false)
            , row(0)
            , col(0)
            , baseline(0)
        {
            memset(&content, 0, sizeof(RECT));
            memset(&boundary, 0, sizeof(RECT));
        }

        void Offset(int x, int y)
        {
            OffsetRect(&content, x, y);
            OffsetRect(&boundary, x, y);
            col += x;
            row += y;
            baseline += y;
        }
    };

    struct AdjustParam
    {
        int x;
        int y;
        bool vertical;

        AdjustParam()
            : x(0)
            , y(0)
            , vertical(false) {}
    };



    struct LayoutRect
    {
        RECT boundary;
        RECT border;
        RECT content;

        LayoutRect()
        {
            Reset();
        }

        void Reset()
        {
            memset(&boundary, 0, sizeof(RECT));
            memset(&border, 0, sizeof(RECT));
            memset(&content, 0, sizeof(RECT));
        }
    };
}

#endif