#include "win/dc.h"

namespace win
{
    WinDC::WinDC()
    {
        type_ = DEFAULT_DC;
        hdc_ = ::GetDC(NULL);
        hwnd_ = NULL;
    }

    WinDC::WinDC(HWND hwnd)
    {
        type_ = HANDLER_DC;
        hdc_ = ::GetDC(hwnd);
        hwnd_ = hwnd;
    }

    WinDC::~WinDC()
    {
        if (type_ == DEFAULT_DC)
        {
            ::ReleaseDC(NULL, hdc_);
        }
        else if (type_ == HANDLER_DC)
        {
            ::ReleaseDC(hwnd_, hdc_);
        }

        hwnd_ = 0;
        hdc_ = 0;
    }

    WinMemoryDC::WinMemoryDC()
    {
        type_ = DEFAULT_DC;
        hdc_ = ::GetDC(NULL);
        mem_dc_ = ::CreateCompatibleDC(hdc_);
        bitmap_ = NULL;
    }

    WinMemoryDC::WinMemoryDC(HDC hdc)
    {
        type_ = COMPATIBLE_DC;
        hdc_ = hdc;
        mem_dc_ = ::CreateCompatibleDC(hdc_);
        bitmap_ = NULL;
    }

    WinMemoryDC::WinMemoryDC(HBITMAP bitmap)
    {
        type_ = DEFAULT_DC | BITMAP_DC;
        hdc_ = ::GetDC(NULL);
        mem_dc_ = ::CreateCompatibleDC(hdc_);
        bitmap_ = (HBITMAP)::SelectObject(mem_dc_, bitmap);
    }

    WinMemoryDC::~WinMemoryDC()
    {
        if ((type_ & BITMAP_DC) == BITMAP_DC)
        {
            ::SelectObject(mem_dc_, bitmap_);
        }

        ::DeleteDC(mem_dc_);

        if ((type_ & DEFAULT_DC) == DEFAULT_DC)
        {
            ::ReleaseDC( NULL, hdc_ );
        }
    }
}