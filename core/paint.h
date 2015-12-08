#ifndef __core_paint_h__
#define __core_paint_h__

#include "base/def.h"
#include "core/types.h"

#include <string>

namespace core
{
    class StCanvas
    {
    public:
        StCanvas() {}
        virtual ~StCanvas() {}
    };



    class StPainterImpl
    {
    public:
        StPainterImpl();
        virtual ~StPainterImpl();

        void Attach(StCanvas* canvas);
        StCanvas* Detach();

        virtual int SetClipRect(
            const RECT &rect)
        {
            return 0;
        }

        virtual bool ClearClipRect(
            int clip)
        {
            return false;
        }

        virtual void DrawRectangle(
            const RECT &rect,
            DWORD style,
            core::Color color,
            int alpha) {}

        virtual void FillRectangle(
            const RECT &rect,
            core::Color color,
            int alpha) {}

        virtual void CalculateImage(
            const std::wstring &file,
            int* width,
            int* height) {}

        virtual void DrawImage(
            const std::wstring &file,
            const RECT &rect,
            int alpha) {}

        virtual void DrawFrameGridImage(
            const std::wstring &file,
            const RECT &rect,
            const RECT &grid,
            int frame_length,
            int frame_index,
            int alpha) {}

        virtual void SetTextFont(
            const std::wstring &family,
            const core::Unit &size,
            DWORD format) {}

        virtual void DrawString(
            const std::wstring &text,
            const RECTF &rect,
            core::Color color,
            DWORD align,
            int alpha) {}

        virtual int GetCharacterPosition(
            const std::wstring &text,
            float &width)
        {
            return 0;
        }

        virtual void DrawLine(
            POINT start,
            POINT stop,
            int nStrokeWidth,
            core::Color color,
            DWORD style,
            int alpha) {}

        virtual void DrawRoundRect(
            const RECT& rect,
            int nRadiusWidth, 
            int nRadiusHeight,
            core::Color crBackground,
            int alpha) {}

        virtual void DrawRoundBorder(
            const RECT& rect,
            int nRadiusWidth, 
            int nRadiusHeight,
            DWORD styleBorder,
            int nBoderWidth,
            core::Color crBorder,
            int alpha) {}

    protected:
        StCanvas* canvas_;

    private:
        DISABLE_COPY_AND_ASSIGN(StPainterImpl)
    };



    class StPainter
    {
    public:
        StPainter();
        virtual ~StPainter();

        virtual int SetClipRect(
            const RECT &rect);

        virtual bool ClearClipRect(
            int clip);

        virtual void DrawBorder(
            const RECT &rect,
            DWORD border_style,
            int border_width,
            core::Color border_color,
            int alpha);

        virtual void DrawBackground(
            const RECT &rect,
            core::Color background_color,
            int alpha);

        virtual void CalculateImage(
            const std::wstring& file,
            int* width,
            int* height);

        virtual void DrawImage(
            const std::wstring &file,
            const RECT &rect,
            int alpha);

        virtual void DrawGridImage(
            const std::wstring &file,
            const RECT &rect,
            const RECT &grid,
            int alpha);

        virtual void DrawFrameImage(
            const std::wstring &file,
            const RECT &rect,
            int frame_length,
            int frame_index,
            int alpha);

        virtual void DrawFrameGridImage(
            const std::wstring &file,
            const RECT &rect,
            const RECT &grid,
            int frame_length,
            int frame_index,
            int alpha);

        virtual void DrawString(
            const std::wstring &text,
            const RECTF &rect,
            const std::wstring &family,
            const core::Unit &size,
            DWORD format,
            core::Color color,
            DWORD align,
            int alpha);

        virtual int GetCharacterPosition(
            const std::wstring &text,
            float &width,
            const std::wstring &family,
            const core::Unit &size,
            DWORD format);

        virtual void DrawLine(
            POINT start,
            POINT stop,
            int nStrokeWidth,
            core::Color color,
            DWORD style,
            int alpha);

        virtual void DrawRoundBorder(
            const RECT& rect,
            int nRadiusWidth, 
            int nRadiusHeight,
            DWORD styleBorder,
            int nBoderWidth,
            core::Color crBorder,
            int alpha);

        virtual void DrawRoundRect(
            const RECT& rect,
            int nRadiusWidth, 
            int nRadiusHeight,
            core::Color crBackground,
            int alpha);

    protected:
        virtual StPainterImpl* GetPainterImpl() = 0;

    private:
        DISABLE_COPY_AND_ASSIGN(StPainter)
    };
}

#endif