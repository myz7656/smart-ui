#include "core/controls/hr.h"

#include "win/gdi_painter.h"

namespace core
{
    IMPLY_DYNAMIC_CREATE(StHr, "hr")

    StHr::StHr()
    {
        SetFilterEvent(true);
    }

    StHr::~StHr() {}

    LRESULT StHr::OnPaint(WPARAM wparam, LPARAM lparam, bool*)
    {
        StPainter* painter = (StPainter*)wparam;
        RECT* clip = (RECT*)lparam;
        if (!painter || !clip)
        {
            return FALSE;
        }

        DrawLine(painter, clip);

        return TRUE;
    }

    void StHr::DrawLine(StPainter* painter, RECT* clip)
    {
        StStyle& style = GetStyle(ActionState());

        int border_width = style.BorderWidth();
        if (border_width > 0)
        {
            DWORD border_style = style.BorderStyle();
            core::Color border_color = style.BorderColor();
            int alpha = Alpha();
            RECT rcContent = layout_rect_.content;
            POINT start, stop;
            start.x = rcContent.left;
            start.y = rcContent.top;
            stop.x = (rcContent.right - rcContent.left == border_width) ? start.x : rcContent.right;
            stop.y = (rcContent.bottom - rcContent.top == border_width) ? start.y : rcContent.bottom;
            painter->DrawLine(start, stop, border_width, 
                border_color, border_style, alpha);
        }
    }

    StObject* StHr::Clone()
    {
        StHr* hr = new StHr;
        if (!hr)
        {
            return 0;
        }

        CopyTo(hr);
        return hr;
    }

    LRESULT StHr::OnCalculate(WPARAM wparam, LPARAM lparam, bool* handled)
    {
        if (!StWidget::OnCalculate(wparam, lparam, handled))
        {
            return FALSE;
        }

        BoxParam* box_param = (BoxParam*)lparam;
        StStyle& style = LayoutStyle();

        if (!style.HasAttribute(STYLE_PROPERTY_HEIGHT))
        {
            box_param->height = style.BorderWidth();
        }
        else if (!style.HasAttribute(STYLE_PROPERTY_WIDTH))
        {
            box_param->width = style.BorderWidth();
        }

        return TRUE;
    }
}