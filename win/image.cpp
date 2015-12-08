#include "win/image.h"
#include "win/dc.h"

namespace win
{
    Image::Image()
        : width_(0)
        , height_(0)
        , bitmap_(NULL)
        , bytes_(NULL) {}

    Image::~Image() {}

    BOOL Image::Copy(Image* image)
    {
        if (!image)
        {
            return FALSE;
        }

        if(image->bitmap_)
        {
            width_ = image->width_;
            height_ = image->height_;
            bitmap_ = image->bitmap_;
            bytes_ = image->bytes_;
            return TRUE;
        }

        return FALSE;
    }

    BOOL Image::CreateFromImage(Gdiplus::Image * image)
    {
        if(!image)
        {
            return FALSE;
        }

        int width = image->GetWidth();
        int height = image->GetHeight();

        if (!CreateDIBSection(width, height))
        {
            return FALSE;
        }

        WinMemoryDC mem_dc(bitmap_);
        Gdiplus::Graphics g((HDC)mem_dc);
        g.DrawImage(image,
                    Gdiplus::Rect(0, 0, width, height),
                    0, 0, width, height,
                    Gdiplus::UnitPixel );

        return TRUE;
    }

    BOOL Image::CreateDIBSection(int width, int height)
    {
        if (width <= 0 || height <= 0)
        {
            return FALSE;
        }

        BITMAPINFO bmi;
        ZeroMemory(&bmi, sizeof(BITMAPINFO));
        bmi.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
        bmi.bmiHeader.biWidth = width;
        bmi.bmiHeader.biHeight = height;
        bmi.bmiHeader.biPlanes = 1;
        bmi.bmiHeader.biBitCount = 32;
        bmi.bmiHeader.biCompression = BI_RGB;
        bmi.bmiHeader.biSizeImage = width * height * 4;

        if (bitmap_ != NULL)
        {
            ::DeleteObject(bitmap_);
        }

        width_ = 0;
        height_ = 0;
        bytes_ = NULL;
        bitmap_ = NULL;

        BYTE* bits = NULL;
        bitmap_ = ::CreateDIBSection(NULL, (BITMAPINFO *)&bmi, DIB_RGB_COLORS, (VOID**)&bits, NULL, 0);

        if (!bits)
        {
            return FALSE;
        }

        width_ = width;
        height_ = height;
        bytes_ = bits;
        ::memset((void *)bytes_, 0, width_ * height_ * 4);

        return TRUE;
    }

    BOOL Image::Release()
    {
        if (bitmap_)
            ::DeleteObject(bitmap_);

        bitmap_ = 0;
        width_ = 0;
        height_ = 0;
        bytes_ = 0;

        return TRUE;
    }

    BOOL Image::SetAlpha(BYTE alpha, DWORD color)
    {
        if (!bytes_)
        {
            return FALSE;
        }

        DWORD *pixel = (DWORD*)bytes_;
        int pixel_count = height_ * width_;

        for (int i = 0; i < pixel_count; i++, pixel++)
        {
            if (*pixel == color)
                ((PBYTE)pixel)[3] = alpha;
        }

        return TRUE;
    }

    BOOL Image::TileBlt(HDC hdc, RECT dest, int alpha)
    {
        WinMemoryDC MemDC( bitmap_ );

        int x = dest.left;
        int y = dest.top;
        int width = dest.right - dest.left;
        int height = dest.bottom - dest.top;

        BLENDFUNCTION blend;
        blend.BlendOp = AC_SRC_OVER;
        blend.BlendFlags = 0;
        blend.SourceConstantAlpha = (BYTE)alpha;
        blend.AlphaFormat = AC_SRC_ALPHA;

        ::AlphaBlend( hdc, x, y, width, height, MemDC, 0, 0, width_, height_, blend );

        return TRUE;
    }

    BOOL Image::GridBlt(HDC hdc, RECT dest, RECT grid, int alpha)
    {
        return FrameGridBlt(hdc, dest, grid, 0, 0, alpha);
    }

    BOOL Image::FrameBlt(HDC hdc, RECT dest, int length, int index, int alpha)
    {
        RECT grid = {0};
        return FrameGridBlt(hdc, dest, grid, length, index, alpha);
    }

    BOOL Image::FrameGridBlt(HDC hdc, RECT dest, RECT grid, int length, int index, int alpha)
    {
        WinMemoryDC MemDC(bitmap_);

        BLENDFUNCTION blend;
        blend.BlendOp = AC_SRC_OVER;
        blend.BlendFlags = 0;
        blend.SourceConstantAlpha = (BYTE)alpha;
        blend.AlphaFormat = AC_SRC_ALPHA;

        if (length == 0)
        {
            length = width_;
        }

        int count = width_ / length;
        index = index % count;

        int left = index * length;
        int top = 0;
        int right = (index + 1) * length;
        int bottom = height_;

        int grid_left = grid.left;
        int grid_top = grid.top;
        int grid_right = grid.right;
        int grid_bottom = grid.bottom;

        if (grid_left > 0 && grid_top > 0)
        {
            ::AlphaBlend(hdc,
                dest.left, dest.top, grid_left, grid_top,
                MemDC,
                left, top, grid_left, grid_top,
                blend);
        }

        if (grid_top > 0 && grid_right > 0)
        {
            ::AlphaBlend(hdc,
                dest.right - grid_right, dest.top, grid_right, grid_top,
                MemDC,
                right - grid_right, top, grid_right, grid_top,
                blend);
        }

        if (grid_left > 0 && grid_bottom > 0)
        {
            ::AlphaBlend(hdc,
                dest.left, dest.bottom - grid_bottom, grid_left, grid_bottom,
                MemDC,
                left, bottom - grid_bottom, grid_left, grid_bottom,
                blend);
        }

        if (grid_right > 0 && grid_bottom > 0)
        {
            ::AlphaBlend(hdc,
                dest.right - grid_right, dest.bottom - grid_bottom, grid_right, grid_bottom,
                MemDC,
                right - grid_right, bottom - grid_bottom, grid_right, grid_bottom,
                blend);
        }

        if (grid_left > 0)
        {
            int heightDest = dest.bottom - dest.top - grid_top - grid_bottom;
            int heightImage = bottom - top - grid_top - grid_bottom;
            ::AlphaBlend(hdc,
                dest.left, dest.top + grid_top, grid_left, heightDest,
                MemDC,
                left, top + grid_top, grid_left, heightImage,
                blend);
        }

        if (grid_top > 0)
        {
            int widthDest = dest.right - dest.left - grid_left - grid_right;
            int widthImage = right - left - grid_left - grid_right;
            ::AlphaBlend(hdc,
                dest.left + grid_left, dest.top, widthDest, grid_top,
                MemDC,
                left + grid_left, top, widthImage, grid_top,
                blend);
        }

        if (grid_right > 0)
        {
            int heightDest = dest.bottom - dest.top - grid_top - grid_bottom;
            int heightImage = bottom - top - grid_top - grid_bottom;
            ::AlphaBlend(hdc,
                dest.right - grid_right, dest.top + grid_top, grid_right, heightDest,
                MemDC,
                right - grid_right, top + grid_top, grid_right, heightImage,
                blend);
        }

        if (grid_bottom > 0)
        {
            int widthDest = dest.right - dest.left - grid_right - grid_left;
            int widthImage = right - left - grid_right - grid_left;
            ::AlphaBlend(hdc,
                dest.left + grid_left, dest.bottom - grid_bottom, widthDest, grid_bottom, 
                MemDC,
                left + grid_left, bottom - grid_bottom, widthImage, grid_bottom,
                blend);
        }

        if (true)
        {
            int widthDest = dest.right - dest.left - grid_left - grid_right;
            int widthImage = right - left - grid_left - grid_right;
            int heightDest = dest.bottom - dest.top - grid_top - grid_bottom;
            int heightImage = bottom - top - grid_top - grid_bottom;
            ::AlphaBlend(hdc,
                dest.left + grid_left, dest.top + grid_top, widthDest, heightDest, 
                MemDC,
                left + grid_left, top + grid_top, widthImage, heightImage,
                blend);
        }

        return TRUE;
    }
}