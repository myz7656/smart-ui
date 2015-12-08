#ifndef __win_dc_h__
#define __win_dc_h__

#include <windows.h>
#include <tchar.h>

#include <string>
#include <atlstr.h>

namespace win
{
    class WinDC
    {
    public:
        enum Type
        {
            DEFAULT_DC       = 0x00000001,
            HANDLER_DC       = 0x00000002
        };

        WinDC();
        WinDC(HWND hwnd);

        ~WinDC();

        operator HDC() { return hdc_; }

    private:
        Type    type_;
        HWND    hwnd_;
        HDC     hdc_;
    };

    class WinMemoryDC
    {
    public:
        enum Type
        {
            DEFAULT_DC       = 0x00000001,
            COMPATIBLE_DC    = 0x00000002,
            BITMAP_DC        = 0x00000004
        };

        WinMemoryDC();
        WinMemoryDC(HDC hdc);
        WinMemoryDC(HBITMAP bitmap);

        ~WinMemoryDC();

        operator HDC(){ return mem_dc_; }

    private:
        DWORD   type_;
        HDC     mem_dc_;
        HDC     hdc_;
        HBITMAP bitmap_;
    };
}

#endif