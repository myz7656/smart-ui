#include "core/controls/span.h"

#include "win/gdi_painter.h"

namespace core
{
    const char* WIDGET_PROPERTY_INNER_TEXT      = "inner-text";

    IMPLY_DYNAMIC_CREATE(StSpan, "span")

    BEGIN_ATTRIBUTE_MAP(StSpan)
        ATTRIBUTE(WIDGET_PROPERTY_INNER_TEXT, StAttributeWString, ATTRIBUTE_TRAIT_BOTH)
        CHAIN_ATTRIBUTE_MAP(StWidget)
    END_ATTRIBUTE_MAP()

    StSpan::StSpan()
    {
        SetFilterEvent(true);
    }

    StSpan::~StSpan() {}

    std::wstring StSpan::InnerText()
    {
        std::wstring inner_text;
        GetAttribute(WIDGET_PROPERTY_INNER_TEXT, &inner_text);
        return inner_text;
    }

    StWidget* StSpan::HitTest(int x, int y)
    {
        if (IsFilterEvent() || !IsVisible())
        {
            return 0;
        }

        for (VectorTokens::iterator iter = text_tokens_.begin(); iter != text_tokens_.end(); iter++)
        {
            RECT rect = RECTF2RECT(iter->rect);
            POINT pt = {x, y};
            if (::PtInRect(&rect, pt))
            {
                return this;
            }
        }
        return 0;
    }

    void StSpan::Move(int x, int y, bool onlyChild)
    {
        for (VectorTokens::iterator iter = text_tokens_.begin(); iter != text_tokens_.end(); iter++)
        {
            iter->rect.X += x;
            iter->rect.Y += y;
        }
    }

    void StSpan::SetInnerText(const std::wstring& text)
    {
        AddAttribute(WIDGET_PROPERTY_INNER_TEXT, text);
        text_tokens_.clear();
    }

    LRESULT StSpan::OnLayout(WPARAM wparam, LPARAM, bool*)
    {
        LayoutParam* layout_param = (LayoutParam *)wparam;
        if (!layout_param)
        {
            return FALSE;
        }

        BoxParam box_param;
        LRESULT result = Dispatch(ST_CALCULATE, (WPARAM)layout_param, (LPARAM)&box_param);
        if (!result)
        {
            return FALSE;
        }

        LayoutLeft(layout_param, box_param);
        LayoutText(layout_param);
        LayoutRight(layout_param, box_param);

        return TRUE;
    }

    LRESULT StSpan::OnPaint(WPARAM wparam, LPARAM lparam, bool*)
    {
        StPainter* painter = (StPainter*)wparam;
        RECT* clip = (RECT*)lparam;
        if (!painter || !clip)
        {
            return FALSE;
        }

        DrawInnerText(painter, clip);

        return TRUE;
    }

    void StSpan::LayoutLeft(LayoutParam* layout_param, BoxParam& box_param)
    {
        layout_rect_.Reset();

        BeforeInlineLayout(*this, *layout_param, box_param);
    }

    void StSpan::LayoutText(LayoutParam* layout_param)
    {
        std::wstring inner_text = InnerText();
        if (inner_text.empty())
        {
            return;
        }

        win::WinPainter painter(0);

        StStyle& style = LayoutStyle();

        int line_height = style.LineHeight();
        std::wstring font_family = style.FontFamily();
        Unit font_size = style.FontSize();
        DWORD font_style = style.FontStyle();

        float rest_width = (float)(layout_param->content.right - layout_param->col);
        int max_width = layout_param->content.right - layout_param->content.left;

        layout_param->baseline = layout_param->row + line_height;

        TextToken token;
        int begin = 0;
        int position = 0;

        text_tokens_.clear();

        do
        {
            std::wstring subValue = inner_text.substr(begin);
            position = painter.GetCharacterPosition(subValue, rest_width, font_family, font_size, font_style);
            if (position == 0)
            {
                break;
            }

            token.rect.X = (float)layout_param->col;
            token.rect.Y = (float)layout_param->row;
            token.rect.Width = rest_width;
            token.rect.Height = layout_param->baseline - token.rect.Y;

            token.begin = begin;
            token.end = position;
            text_tokens_.push_back(token);

            begin += position;

            if (position == subValue.size())
            {
                layout_param->col += (int)(rest_width + 0.5);

                layout_param->boundary.left = min(layout_param->boundary.left, layout_param->col);
                layout_param->boundary.right = max(layout_param->boundary.right, layout_param->col);
                layout_param->boundary.top = min(layout_param->boundary.top, layout_param->row);
                layout_param->boundary.bottom = max(layout_param->boundary.bottom, layout_param->baseline);
                break;
            }
            else
            {
                layout_param->boundary.left = min(layout_param->boundary.left, layout_param->content.left);
                layout_param->boundary.right = max(layout_param->boundary.right, layout_param->col + (int)(rest_width + 0.5));

                layout_param->col = layout_param->content.left;
                layout_param->row += line_height;
                layout_param->baseline += line_height;

                layout_param->boundary.top = min(layout_param->boundary.top, layout_param->row);
                layout_param->boundary.bottom = max(layout_param->boundary.bottom, layout_param->baseline);

                rest_width = (float)max_width;
            }

        } while (true);
    }

    void StSpan::LayoutRight(LayoutParam* layout_param, BoxParam& box_param)
    {
        AfterInlineLayout(*this, *layout_param, box_param);
    }

    void StSpan::DrawInnerText(StPainter* painter, RECT* clip)
    {
        std::wstring inner_text = InnerText();
        if (inner_text.empty())
        {
            return;
        }

        StStyle& style = GetStyle(ActionState());

        int line_height = style.LineHeight();
        std::wstring font_family = style.FontFamily();
        Unit font_size = style.FontSize();
        DWORD font_style = style.FontStyle();
        bool underline = style.IsUnderline();
        core::Color color = style.TextColor();
        DWORD text_alignment = style.TextAlignment();

        int alpha = 0;
        GetAttribute(WIDGET_PROPERTY_ALPHA, &alpha, 255);

        if (underline)
        {
            font_style |= core::FONT_STYLE_UNDERLINE;
        }
        for (VectorTokens::iterator iter = text_tokens_.begin(); iter != text_tokens_.end(); iter++)
        {
            RECTF rectf_clip = RECT2RECTF(*clip);
            if (!rectf_clip.IntersectsWith(rectf_clip))
            {
                continue;
            }

            int nclip = painter->SetClipRect(RECTF2RECT(iter->rect));

            std::wstring sub_text = inner_text.substr(iter->begin, iter->end - iter->begin);
            painter->DrawString(
                sub_text,
                iter->rect,
                font_family,
                font_size,
                font_style,
                color,
                text_alignment,
                alpha);

            painter->ClearClipRect(nclip);
        }
    }

    StObject* StSpan::Clone()
    {
        StSpan* span = new StSpan;
        if (!span)
        {
            return 0;
        }

        CopyTo(span);
        return span;
    }

    void StSpan::CopyTo( StSpan* span )
    {
        StWidget::CopyTo(span);
        span->text_tokens_ = text_tokens_;
    }

    bool StSpan::BeforeInlineLayout(StWidget& widget, LayoutParam& layout, BoxParam& box)
    {
        StStyle& style = widget.LayoutStyle();
        int line_height = style.LineHeight();
        int max_width = layout.content.right - layout.content.left;
        int rest_width = layout.content.right - layout.col;

        layout.baseline = layout.row + line_height;

        int size = box.border + box.margin_left + box.padding_left;
        while (size >= rest_width && rest_width != 0)
        {
            layout.col = layout.content.left;
            layout.row += line_height;
            layout.baseline += line_height;

            layout.boundary.left = min(layout.boundary.left, layout.content.left);
            layout.boundary.right = max(layout.boundary.right, layout.content.right);
            layout.boundary.top = min(layout.boundary.top, layout.row);
            layout.boundary.bottom = max(layout.boundary.bottom, layout.baseline);

            size -= rest_width;
            rest_width = max_width;
        }

        layout.col += size;

        layout.boundary.left = min(layout.boundary.left, layout.col);
        layout.boundary.right = max(layout.boundary.right, layout.col);

        return true;
    }

    bool StSpan::AfterInlineLayout(StWidget& widget, LayoutParam& layout, BoxParam& box)
    {
        StStyle& style = widget.LayoutStyle();

        int line_height = style.LineHeight();
        int max_width = layout.content.right - layout.content.left;
        int rest_width = layout.content.right - layout.col;

        int size = box.border + box.margin_right + box.padding_right;
        while (size >= rest_width && rest_width != 0)
        {
            layout.col = layout.content.left;
            layout.row += line_height;
            layout.baseline += line_height;

            layout.boundary.left = min(layout.boundary.left, layout.content.left);
            layout.boundary.right = max(layout.boundary.right, layout.content.right);
            layout.boundary.top = min(layout.boundary.top, layout.row);
            layout.boundary.bottom = max(layout.boundary.bottom, layout.baseline);

            size -= rest_width;
            rest_width = max_width;
        }

        layout.col += size;

        layout.boundary.left = min(layout.boundary.left, layout.col);
        layout.boundary.right = max(layout.boundary.right, layout.col);

        return true;
    }
}