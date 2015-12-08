#include "core/paint.h"
#include "core/widget.h"

namespace core
{
    StPainterImpl::StPainterImpl()
        : canvas_(0) {}

    StPainterImpl::~StPainterImpl() {}

    void StPainterImpl::Attach(StCanvas* canvas)
    {
        canvas_ = canvas;
    }

    StCanvas* StPainterImpl::Detach()
    {
        StCanvas* ret = canvas_;
        canvas_ = 0;
        return ret;
    }



    StPainter::StPainter() {}

    StPainter::~StPainter() {}

    int StPainter::SetClipRect(
        const RECT &rect)
    {
        StPainterImpl *impl = GetPainterImpl();
        if (impl)
        {
            return impl->SetClipRect(rect);
        }
        return 0;
    }

    bool StPainter::ClearClipRect(
        int clip)
    {
        StPainterImpl *impl = GetPainterImpl();
        if (impl)
        {
            return impl->ClearClipRect(clip);
        }

        return false;
    }

    void StPainter::DrawBorder(
        const RECT &rect,
        DWORD border_style,
        int border_width,
        core::Color border_color,
        int alpha)
    {
        StPainterImpl *impl = GetPainterImpl();
        if (impl)
        {
            RECT rect_draw = rect;
            for (int i = 0; i < border_width; i++)
            {
                impl->DrawRectangle(rect_draw, border_style, border_color, alpha);
                rect_draw.left += 1;
                rect_draw.top += 1;
                rect_draw.right -= 1;
                rect_draw.bottom -= 1;
            }
        }
    }

    void StPainter::DrawBackground(
        const RECT &rect,
        core::Color background_color,
        int alpha)
    {
        StPainterImpl *impl = GetPainterImpl();
        if (impl)
        {
            impl->FillRectangle(rect, background_color, alpha);
        }
    }

    void StPainter::CalculateImage(
        const std::wstring& file,
        int* width,
        int* height)
    {
        StPainterImpl* impl = GetPainterImpl();
        if (impl)
        {
            impl->CalculateImage(file, width, height);
        }
    }

    void StPainter::DrawImage(
        const std::wstring &file,
        const RECT &rect,
        int alpha)
    {
        StPainterImpl *impl = GetPainterImpl();
        if (impl)
        {
            impl->DrawImage(file, rect, alpha);
        }
    }

    void StPainter::DrawGridImage(
        const std::wstring &file,
        const RECT &rect,
        const RECT &grid,
        int alpha)
    {
        StPainterImpl *impl = GetPainterImpl();
        if (impl)
        {
            impl->DrawFrameGridImage(file, rect, grid, 0, 0, alpha);
        }
    }

    void StPainter::DrawFrameImage(
        const std::wstring &file,
        const RECT &rect,
        int frame_length,
        int frame_index,
        int alpha)
    {
        StPainterImpl *impl = GetPainterImpl();
        if (impl)
        {
            RECT grid = {0};
            impl->DrawFrameGridImage(file, rect, grid, frame_length, frame_index, alpha);
        }
    }

    void StPainter::DrawFrameGridImage(
        const std::wstring &file,
        const RECT &rect,
        const RECT &grid,
        int frame_length,
        int frame_index,
        int alpha)
    {
        StPainterImpl *impl = GetPainterImpl();
        if (impl)
        {
            impl->DrawFrameGridImage(file, rect, grid, frame_length, frame_index, alpha);
        }
    }

    void StPainter::DrawString(
        const std::wstring &text,
        const RECTF &rect,
        const std::wstring &family,
        const core::Unit &size,
        DWORD format,
        core::Color color,
        DWORD align,
        int alpha)
    {
        StPainterImpl *impl = GetPainterImpl();
        if (impl)
        {
            impl->SetTextFont(family, size, format);
            impl->DrawString(text, rect, color, align, alpha);
        }
    }

    int StPainter::GetCharacterPosition(
        const std::wstring &text,
        float &width,
        const std::wstring &family,
        const core::Unit &size,
        DWORD format)
    {
        StPainterImpl *impl = GetPainterImpl();
        if (impl)
        {
            impl->SetTextFont(family, size, format);
            return impl->GetCharacterPosition(text, width);
        }
        return 0;
    }

    void StPainter::DrawLine( 
        POINT start, 
        POINT stop, 
        int nStrokeWidth, 
        core::Color color, 
        DWORD style, 
        int alpha )
    {
        StPainterImpl *impl = GetPainterImpl();
        if (impl)
        {
            impl->DrawLine(start, stop, nStrokeWidth, color, style, alpha);
        }
    }

    void StPainter::DrawRoundBorder( const RECT& rect, int nRadiusWidth, int nRadiusHeight, DWORD styleBorder, int nBoderWidth, core::Color crBorder, int alpha )
    {
        StPainterImpl *impl = GetPainterImpl();
        if (impl)
        {
            impl->DrawRoundBorder(rect, nRadiusWidth, nRadiusHeight, styleBorder, nBoderWidth, crBorder, alpha);
        }
    }

    void StPainter::DrawRoundRect( const RECT& rect, int nRadiusWidth, int nRadiusHeight, core::Color crBackground, int alpha )
    {
        StPainterImpl *impl = GetPainterImpl();
        if (impl)
        {
            impl->DrawRoundRect(rect, nRadiusWidth, nRadiusHeight, crBackground, alpha);
        }
    }


}