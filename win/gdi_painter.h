#ifndef __win_gdi_painter_h__
#define __win_gdi_painter_h__

#include "base/def.h"
#include "core/paint.h"
#include "core/types.h"

namespace win
{
    class HDCCanvas : public core::StCanvas
    {
    public:
        HDCCanvas();
        ~HDCCanvas();

        void Attatch(HDC hdc);
        HDC Detach();
        HDC GetDC();

    private:
        HDC hdc_;
    };



    class WinPainterImpl : public core::StPainterImpl
    {
    public:
        WinPainterImpl(BOOL bLayered);
        ~WinPainterImpl();

    public:
        virtual int SetClipRect(
            const RECT &rect);

        virtual bool ClearClipRect(
            int clip);

        virtual void DrawRectangle(
            const RECT &rect,
            DWORD style,
            core::Color color,
            int alpha);

        virtual void FillRectangle(
            const RECT &rect,
            core::Color color,
            int alpha);

        virtual void CalculateImage(
            const std::wstring &file,
            int* width,
            int* height);

        virtual void DrawImage(
            const std::wstring &file,
            const RECT &rect,
            int alpha);

        virtual void DrawFrameGridImage(
            const std::wstring &file,
            const RECT &rect,
            const RECT &grid,
            int frame_length,
            int frame_index,
            int alpha);

        virtual void SetTextFont(
            const std::wstring &family,
            const core::Unit &size,
            DWORD format);

        virtual void DrawString(
            const std::wstring &text,
            const core::RECTF &rect,
            core::Color color,
            DWORD align,
            int alpha);

        virtual int GetCharacterPosition(
            const std::wstring &text,
            float &width);

        virtual void DrawLine(
            POINT start,
            POINT stop,
            int nStrokeWidth,
            core::Color color,
            DWORD style,
            int alpha);

        virtual void DrawRoundRect(
            const RECT& rect,
            int nRadiusWidth, 
            int nRadiusHeight,
            core::Color crBackground,
            int alpha
            );

        virtual void DrawRoundBorder(
            const RECT& rect,
            int nRadiusWidth, 
            int nRadiusHeight,
            DWORD styleBorder,
            int nBoderWidth,
            core::Color crBorder,
            int alpha);


//         virtual void DrawPath(
//             );

    protected:
        HDC GetDC();

        void DrawStringNormal( 
            const RECT & rect, 
            core::Color &color, 
            HFONT font, 
            const std::wstring &text, 
            DWORD format);

        void DrawStringLayered( 
            const RECT &rect, 
            core::Color &color, 
            HFONT font, 
            const std::wstring &text, 
            DWORD format, 
            int alpha);

    private:
        ULONG_PTR gdi_token_;

        std::wstring font_family_;
        int          font_size_;
        DWORD        font_style_;
        BOOL         layered_;

        static const int DEFAULT_FONT_SIZE   = 12;
        static const int DEFAULT_FONT_RATE   = 100;
        static const int MAX_ALPHA           = 255;
        static const int DEFAULT_BLACK_COLOR = RGB(0x01, 0x01, 0x01);

    private:
        DISABLE_COPY_AND_ASSIGN(WinPainterImpl)
    };



    class WinPainter : public core::StPainter
    {
    public:
        WinPainter(HDCCanvas* canvas, BOOL bLayered= FALSE);
        ~WinPainter();

    protected:
        void ReleasePainterImpl();
        core::StPainterImpl* GetPainterImpl();

    private:
        WinPainterImpl* impl_;
        HDCCanvas*      canvas_;
        BOOL            layered_;

    private:
        DISABLE_COPY_AND_ASSIGN(WinPainter)
    };
}

#endif