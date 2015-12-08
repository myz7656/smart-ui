#ifndef __core_layout_h__
#define __core_layout_h__

#include "core/types.h"

namespace core
{
    class StWidget;
    class StStyle;



    class LayoutHelper
    {
    public:
        static DWORD GetPositionType(StStyle& style);
        static int GetBaseline(StWidget &widget, bool exclude);
    };



    class ILayout
    {
    public:
        virtual bool CalculateBox(StStyle& style, RECT parent, BoxParam* box) = 0;

        virtual bool BeforeBlockLayout(StWidget& widget, LayoutParam& parent, BoxParam& box, LayoutRect* rect) = 0;
        virtual bool AfterBlockLayout (StWidget& widget, LayoutParam& self) = 0;
    };



    class StreamLayout
        : public ILayout
    {
    public:
        bool CalculateBox(StStyle& style, RECT parent, BoxParam* box);

        bool BeforeBlockLayout(StWidget& widget, LayoutParam& parent, BoxParam& box, LayoutRect* rect);
        bool AfterBlockLayout (StWidget& widget, LayoutParam& self);

    protected:
        bool BeforeAbsoluteBlockLayout(StWidget& widget, LayoutParam& parent, BoxParam& box, LayoutRect* rect);
        bool AfterAbsoluteBlockLayout(StWidget& widget, LayoutParam& self);

        bool BeforeRelativeBlockLayout(StWidget& widget, LayoutParam& parent, BoxParam& box, LayoutRect* rect);
        bool AfterRelativeBlockLayout(StWidget& widget, LayoutParam& self);

        bool DoAlignment(StWidget& widget, LayoutParam& self);
    };
};

#endif