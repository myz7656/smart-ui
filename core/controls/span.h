#ifndef __core_controls_span_h__
#define __core_controls_span_h__

#include "core/def.h"
#include "core/widget.h"

#include <string>
#include <gdiplus.h>

namespace core
{
    extern const char* WIDGET_PROPERTY_INNER_TEXT;

    class StSpan : public StWidget
    {
    public:
        DECLARE_DYNAMIC_CREATE(StSpan);

        StSpan();
        ~StSpan();

        virtual StObject* Clone();
        virtual StWidget* HitTest(int x, int y);
        virtual void Move(int x, int y, bool onlyChild);

        std::wstring InnerText();
        void SetInnerText(const std::wstring& text);

    protected:
        DECLARE_ATTRIBUTE_MAP()

        BEGIN_MESSAGE_MAP()
            MESSAGE_HANDLE(ST_LAYOUT, OnLayout)
            MESSAGE_HANDLE(ST_PAINT, OnPaint)
            CHAIN_MESSAGE_MAP(StWidget)
        END_MESSAGE_MAP()

        LRESULT OnLayout(WPARAM, LPARAM, bool*);
        LRESULT OnPaint(WPARAM, LPARAM, bool*);

        void LayoutLeft(LayoutParam* layout_param, BoxParam& box_param);
        void LayoutText(LayoutParam* layout_param);
        void LayoutRight(LayoutParam* layout_param, BoxParam& box_param);

        bool BeforeInlineLayout(StWidget& widget, LayoutParam& layout, BoxParam& box);
        bool AfterInlineLayout(StWidget& widget, LayoutParam& layout, BoxParam& box);

        void DrawInnerText(StPainter* painter, RECT* clip);

        void CopyTo(StSpan* span);

    protected:

        struct TextToken
        {
            RECTF rect;
            int  begin;
            int  end;

            TextToken() : begin(0), end(0) {}
        };

        typedef std::vector<TextToken> VectorTokens;
        VectorTokens text_tokens_;

    private:
        DISABLE_COPY_AND_ASSIGN(StSpan)
    };
};
#endif