#include "win/gdi_painter.h"

#include "base/scoped_ptr.h"
#include "base/singleton.h"

#include "util/string.h"

#include "win/asset_manager.h"
#include "win/image.h"
#include "win/font.h"
#include "win/dc.h"

#include "core/const.h"

#include <gdiplus.h>
#pragma comment(lib, "gdiplus.lib")

namespace win
{
    HDCCanvas::HDCCanvas()
        : hdc_(0) {}

    HDCCanvas::~HDCCanvas() {}

    void HDCCanvas::Attatch(HDC hdc)
    {
        hdc_ = hdc;
    }

    HDC HDCCanvas::Detach()
    {
        HDC hdc_temp = hdc_;
        hdc_ = 0;
        return hdc_temp;
    }

    HDC HDCCanvas::GetDC()
    {
        return hdc_;
    }



    WinPainterImpl::WinPainterImpl(BOOL bLayered)
        : font_style_(core::FONT_STYLE_NORMAL)
        , layered_(bLayered)
    {
        Gdiplus::GdiplusStartupInput gdi_plus_startup_input;
        Gdiplus::GdiplusStartup(&gdi_token_, &gdi_plus_startup_input, NULL);
    }

    WinPainterImpl::~WinPainterImpl()
    {
        Gdiplus::GdiplusShutdown(gdi_token_);
    }

    int WinPainterImpl::SetClipRect(
        const RECT &rect)
    {
        HDC hdc = GetDC();
        if (hdc)
        {
            HRGN clip_rect = ::CreateRectRgn(rect.left, rect.top, rect.right, rect.bottom);
            if (clip_rect)
            {
                int clipSave = ::SaveDC(hdc);
                ::ExtSelectClipRgn(hdc, clip_rect, RGN_AND);
                ::DeleteObject(clip_rect);
                return clipSave;
            }
        }
        return 0;
    }

    bool WinPainterImpl::ClearClipRect(
        int clip)
    {
        ::RestoreDC(GetDC(), clip);
        return true;
    }

    void WinPainterImpl::DrawRectangle(
        const RECT &rect,
        DWORD style,
        core::Color color,
        int alpha)
    {
        win::Image image;
        int width = rect.right - rect.left;
        int height = rect.bottom - rect.top;
        image.CreateDIBSection(width, height);
        {
            WinMemoryDC MemDC(image);
            RECT rect_draw = {0, 0, width, height};
            DWORD format = 0;
            if((style & core::BORDER_STYLE_SOLID) == core::BORDER_STYLE_SOLID)
                format |= PS_SOLID;
            else if((style & core::BORDER_STYLE_DOTTED) == core::BORDER_STYLE_DOTTED)
                format |= PS_DOT;
            else if((style & core::BORDER_STYLE_DASHED) == core::BORDER_STYLE_DASHED)
                format |= PS_DASH;

            HPEN hPen = ::CreatePen(format, 1, RGB(color.rgb_value.r, color.rgb_value.g, color.rgb_value.b));
            HPEN hOld = (HPEN)::SelectObject(MemDC, hPen);
            ::Rectangle(MemDC, 0, 0, width, height);
            ::SelectObject(MemDC, hOld);
            ::DeleteObject(hPen);
        }
        image.SetAlpha(MAX_ALPHA, RGB(color.rgb_value.b, color.rgb_value.g, color.rgb_value.r));

        RECT rect_draw = { rect.left, rect.top, rect.right, rect.bottom };
        image.TileBlt(GetDC(), rect_draw, alpha);
        image.Release();
    }

    void WinPainterImpl::FillRectangle(
        const RECT &rect,
        core::Color color,
        int alpha)
    {
        win::Image image;
        int width = rect.right - rect.left;
        int height = rect.bottom - rect.top;
        image.CreateDIBSection(width, height);
        {
            win::WinMemoryDC mem_dc(image);
            HBRUSH brush = ::CreateSolidBrush(RGB(color.rgb_value.r, color.rgb_value.g, color.rgb_value.b));
            RECT rect_draw = { 0, 0, width, height };
            ::FillRect(mem_dc, &rect_draw, brush);
            ::DeleteObject(brush);
        }
        image.SetAlpha(MAX_ALPHA, RGB(color.rgb_value.b, color.rgb_value.g, color.rgb_value.r));

        RECT rect_draw = { rect.left, rect.top, rect.right, rect.bottom };
        image.TileBlt(GetDC(), rect_draw, alpha);
        image.Release();
    }

    void WinPainterImpl::CalculateImage(
        const std::wstring &file,
        int* width,
        int* height)
    {
        if (file.empty())
        {
            return;
        }

        win::Image image;
        AssetManagerSingleton::Instance().LoadImage(file.c_str(), &image);
        if (width)
        {
            *width = image.Width();
        }
        if (height)
        {
            *height = image.Height();
        }
    }

    void WinPainterImpl::DrawImage(
        const std::wstring &file,
        const RECT &rect,
        int alpha)
    {
        win::Image image;
        AssetManagerSingleton::Instance().LoadImage(file.c_str(), &image);
        RECT rect_dest = { rect.left, rect.top, rect.right, rect.bottom };
        image.TileBlt(GetDC(), rect_dest, alpha);
    }

    void WinPainterImpl::DrawFrameGridImage(
        const std::wstring &file,
        const RECT &rect,
        const RECT &grid,
        int frame_length,
        int frame_index,
        int alpha)
    {
        win::Image image;
        AssetManagerSingleton::Instance().LoadImage(file.c_str(), &image);

        RECT rect_dest = { rect.left, rect.top, rect.right, rect.bottom };
        RECT rect_grid = { grid.left, grid.top, grid.right, grid.bottom };
        image.FrameGridBlt(GetDC(), rect_dest, rect_grid, frame_length, frame_index, alpha);
    }

    void WinPainterImpl::SetTextFont(
        const std::wstring &family,
        const core::Unit &size,
        DWORD format)
    {
        font_family_ = family;
        if (size.pixel)
        {
            font_size_ = size.use;
        }
        else
        {
            font_size_ = DEFAULT_FONT_SIZE + (size.use - DEFAULT_FONT_RATE)/DEFAULT_FONT_RATE;
        }

        font_style_ = format;
    }

    void WinPainterImpl::DrawString(
        const std::wstring &text,
        const core::RECTF &rect,
        core::Color color,
        DWORD align,
        int alpha)
    {
        if (text.empty())
        {
            return;
        }

        float width = rect.Width;
        float height = rect.Height;

        const Gdiplus::StringFormat* string_format = Gdiplus::StringFormat::GenericTypographic();
        Gdiplus::StringFormat format(string_format);

        if ((align & core::HORIZONTAL_ALIGNMENT_CENTER) == core::HORIZONTAL_ALIGNMENT_CENTER)
            format.SetAlignment(Gdiplus::StringAlignmentCenter);
        else if ((align & core::HORIZONTAL_ALIGNMENT_RIGHT) == core::HORIZONTAL_ALIGNMENT_RIGHT)
            format.SetAlignment(Gdiplus::StringAlignmentFar);
        else
            format.SetAlignment(Gdiplus::StringAlignmentNear);

        if ((align & core::VERTICAL_ALIGNMENT_MIDDLE) == core::VERTICAL_ALIGNMENT_MIDDLE)
            format.SetLineAlignment(Gdiplus::StringAlignmentCenter);
        else if ((align & core::VERTICAL_ALIGNMENT_BOTTOM) == core::VERTICAL_ALIGNMENT_BOTTOM)
            format.SetLineAlignment(Gdiplus::StringAlignmentFar);
        else
            format.SetLineAlignment(Gdiplus::StringAlignmentNear);

        Gdiplus::Font* font = FontHelper::CreateGdiplusFont(
            font_family_.c_str(),
            font_size_,
            font_style_ & core::FONT_STYLE_BOLD,
            font_style_ & core::FONT_STYLE_ITALIC,
            font_style_ & core::FONT_STYLE_UNDERLINE);

        if (!font)
        {
            return ;
        }

        Gdiplus::RectF rect_draw(rect);
        Gdiplus::SolidBrush brush(Gdiplus::Color(alpha, color.rgb_value.r, color.rgb_value.g, color.rgb_value.b));

        Gdiplus::Graphics graphics(GetDC());

        format.SetFormatFlags(Gdiplus::StringFormatFlagsNoClip);
        graphics.DrawString(text.c_str(), text.length(), font, rect_draw, &format, &brush);
        
        delete font;
    }

    int WinPainterImpl::GetCharacterPosition(
        const std::wstring &text,
        float &width)
    {
        if (text.empty())
        {
            return 0;
        }

        int length = (int)text.size();
        win::WinMemoryDC mem_dc(GetDC());
        Gdiplus::Graphics graphics(mem_dc);

        Gdiplus::Font* font = FontHelper::CreateGdiplusFont(
            font_family_.c_str(),
            font_size_,
            font_style_ & core::FONT_STYLE_BOLD,
            font_style_ & core::FONT_STYLE_ITALIC,
            font_style_ & core::FONT_STYLE_UNDERLINE);

        if (!font)
        {
            return 0;
        }

        RECT rect_draw = { 0 };

        int index = 0;
        float last = 0;
        LPCWSTR pText = text.c_str();
        for(int i = length; i >= 0; i--)
        {
            Gdiplus::RectF boundingBox;

            ::DrawText(mem_dc, pText, i, &rect_draw, DT_SINGLELINE | DT_LEFT | DT_CALCRECT);
            graphics.MeasureString(text.c_str(), i, font, Gdiplus::PointF(0, 0), Gdiplus::StringFormat::GenericTypographic(), &boundingBox);

            float draw_width = boundingBox.Width;
            if (draw_width > (float)width)
            {
            }
            else
            {
                last = draw_width;
                index = i;
                break;
            }
        }

        width = last;

        delete font;
        return index;
    }

    HDC WinPainterImpl::GetDC()
    {
        if (canvas_)
        {
            HDCCanvas * canvas = static_cast<HDCCanvas*>(canvas_);
            return canvas->GetDC();
        }
        return 0;
    }

    void WinPainterImpl::DrawStringNormal( const RECT & rect, core::Color &color, HFONT font, const std::wstring &text, DWORD format )
    {
        RECT rect_draw = rect;
        int nOldMode = ::SetBkMode(GetDC(), TRANSPARENT);
        COLORREF crOld = ::SetTextColor(GetDC(), RGB(color.rgb_value.r, color.rgb_value.g, color.rgb_value.b));
        HFONT old_font = (HFONT)::SelectObject(GetDC(), font);

        ::DrawText(GetDC(), text.c_str(), text.size(), &rect_draw, format);

        ::SelectObject(GetDC(), old_font);
        ::SetTextColor(GetDC(), crOld);
        ::SetBkMode(GetDC(), nOldMode);
    }

    void WinPainterImpl::DrawStringLayered( const RECT &rect, core::Color &color, HFONT font, const std::wstring &text, DWORD format, int alpha )
    {
        int width = rect.right - rect.left;
        int height = rect.bottom - rect.top;

        win::Image image;
        image.CreateDIBSection(width, height);

        {
            win::WinMemoryDC mem_dc(image);
            ::SetBkMode(mem_dc, TRANSPARENT);
            ::SetTextColor(mem_dc, RGB(color.rgb_value.r, color.rgb_value.g, color.rgb_value.b));

            HFONT old_font = (HFONT)::SelectObject(mem_dc, font);

            RECT rect_draw = {0, 0, width, height};
            ::DrawText(mem_dc, text.c_str(), text.size(), &rect_draw, format);
            ::SelectObject(mem_dc, old_font);
        }

        image.SetAlpha(MAX_ALPHA, RGB(color.rgb_value.b, color.rgb_value.g, color.rgb_value.r));

        RECT rect_draw = { rect.left, rect.top, rect.right, rect.bottom };
        image.TileBlt(GetDC(), rect_draw, alpha);
        image.Release();
    }

    void WinPainterImpl::DrawLine(POINT start, POINT stop, int nStrokeWidth, core::Color color, DWORD style, int alpha)
    {
        int width = abs(stop.x - start.x);
        int height = abs(stop.y - start.y);

        if (width == 0)
        {
            width = nStrokeWidth;
        }

        if (height == 0)
        {
            height = nStrokeWidth;
        }
        win::Image image;
        image.CreateDIBSection(width, height);

        {
            win::WinMemoryDC mem_dc(image);

            DWORD format = 0;
            if((style & core::BORDER_STYLE_SOLID) == core::BORDER_STYLE_SOLID)
                format |= PS_SOLID;
            else if((style & core::BORDER_STYLE_DOTTED) == core::BORDER_STYLE_DOTTED)
                format |= PS_DOT;
            else if((style & core::BORDER_STYLE_DASHED) == core::BORDER_STYLE_DASHED)
                format |= PS_DASH;

            HPEN hPen = ::CreatePen(format, nStrokeWidth, RGB(color.rgb_value.r, color.rgb_value.g, color.rgb_value.b));
            HPEN hOld = (HPEN)::SelectObject(mem_dc, hPen);

            ::MoveToEx(mem_dc, 0, 0, NULL);
            ::LineTo(mem_dc, abs(stop.x - start.x), abs(stop.y - start.y));
            ::SelectObject(mem_dc, hOld);
        }

        image.SetAlpha(MAX_ALPHA, RGB(color.rgb_value.b, color.rgb_value.g, color.rgb_value.r));

        RECT rect_draw = { min(start.x, stop.x), min(start.y, stop.y), min(start.x, stop.x) + width, min(start.y, stop.y) + height};
        image.TileBlt(GetDC(), rect_draw, alpha);
        image.Release();
    }

    void WinPainterImpl::DrawRoundBorder(const RECT& rect, int nRadiusWidth, int nRadiusHeight, DWORD styleBorder, int nBoderWidth, core::Color crBorder, int alpha)
    {
        win::Image image;
        int width = rect.right - rect.left;
        int height = rect.bottom - rect.top;
        image.CreateDIBSection(width, height);
        {
            WinMemoryDC MemDC(image);
            RECT rect_draw = {0, 0, width, height};
            DWORD format = 0;
            if((styleBorder & core::BORDER_STYLE_SOLID) == core::BORDER_STYLE_SOLID)
                format |= PS_SOLID;
            else if((styleBorder & core::BORDER_STYLE_DOTTED) == core::BORDER_STYLE_DOTTED)
                format |= PS_DOT;
            else if((styleBorder & core::BORDER_STYLE_DASHED) == core::BORDER_STYLE_DASHED)
                format |= PS_DASH;

            HPEN hPen = ::CreatePen(format, nBoderWidth, RGB(crBorder.rgb_value.r, crBorder.rgb_value.g, crBorder.rgb_value.b));
            HPEN hOld = (HPEN)::SelectObject(MemDC, hPen);

            ::RoundRect(MemDC, 0, 0, width, height, nRadiusWidth*2, nRadiusHeight*2);

            ::SelectObject(MemDC, hOld);
            ::DeleteObject(hPen);
        }

        image.SetAlpha(MAX_ALPHA, RGB(crBorder.rgb_value.b, crBorder.rgb_value.g, crBorder.rgb_value.r));

        RECT rect_draw = { rect.left, rect.top, rect.right, rect.bottom };
        image.TileBlt(GetDC(), rect_draw, alpha);
        image.Release();
    }

    void WinPainterImpl::DrawRoundRect( const RECT& rect, int nRadiusWidth, int nRadiusHeight, core::Color crBackground, int alpha )
    {
        win::Image image;
        int width = rect.right - rect.left;
        int height = rect.bottom - rect.top;
        image.CreateDIBSection(width, height);
        {
            WinMemoryDC MemDC(image);
            RECT rect_draw = {0, 0, width, height};

            HBRUSH hBrush = ::CreateSolidBrush(RGB(crBackground.rgb_value.r, crBackground.rgb_value.g, crBackground.rgb_value.b));
            HBRUSH hOldBrush = (HBRUSH)::SelectObject(MemDC, hBrush);

            ::RoundRect(MemDC, 0, 0, width, height, nRadiusWidth*2, nRadiusHeight*2);

            ::SelectObject(MemDC, hOldBrush);
            ::DeleteObject(hBrush);
        }

        image.SetAlpha(MAX_ALPHA, RGB(crBackground.rgb_value.b, crBackground.rgb_value.g, crBackground.rgb_value.r));

        RECT rect_draw = { rect.left, rect.top, rect.right, rect.bottom };
        image.TileBlt(GetDC(), rect_draw, alpha);
        image.Release();
    }

    WinPainter::WinPainter(HDCCanvas* canvas, BOOL bLayered/* = FALSE*/)
        : canvas_(canvas)
        , impl_(0)
        , layered_() {}

    WinPainter::~WinPainter()
    {
        ReleasePainterImpl();
    }

    void WinPainter::ReleasePainterImpl()
    {
        if (impl_)
        {
            impl_->Detach();
            delete impl_;
        }
    }

    core::StPainterImpl* WinPainter::GetPainterImpl()
    {
        if (!impl_)
        {
            impl_ = new WinPainterImpl(layered_);
        }

        if (impl_)
        {
            impl_->Attach(canvas_);
        }
        return impl_;
    }
}