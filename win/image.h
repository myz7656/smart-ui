#ifndef __win_image_h__
#define __win_image_h__

#include "base/def.h"

#include <windows.h>
#include <gdiplus.h>
#pragma comment(lib, "gdiplus.lib")

#include <wingdi.h>
#pragma comment(lib, "msimg32.lib")

namespace win
{
    class Image
    {
    public:
        Image();
        ~Image();

        BOOL Copy(Image* image);
        BOOL Release();

        BOOL CreateFromImage(Gdiplus::Image * image);
        BOOL CreateDIBSection(int width, int height);

        BOOL SetAlpha(BYTE alpha, DWORD color);

        BOOL TileBlt(HDC hdc, RECT dest, int alpha);
        BOOL GridBlt(HDC hdc, RECT dest, RECT grid, int alpha);
        BOOL FrameBlt(HDC hdc, RECT dest, int length, int index, int alpha);
        BOOL FrameGridBlt(HDC hdc, RECT dest, RECT grid, int length, int index, int alpha);

        int Width() { return width_; }
        int Height(){ return height_; }
        operator HBITMAP(){ return bitmap_; }

    private:
        int     width_;
        int     height_;
        HBITMAP bitmap_;
        BYTE*   bytes_;

    private:
        DISABLE_COPY_AND_ASSIGN(Image)
    };
}

#endif