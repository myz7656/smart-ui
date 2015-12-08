#ifndef __win_layered_window_h__
#define __win_layered_window_h__

#include "win/window.h"

typedef CWinTraits<WS_POPUP | WS_VISIBLE | WS_CLIPCHILDREN | WS_SYSMENU | WS_MINIMIZEBOX | WS_MAXIMIZEBOX, WS_EX_LAYERED>    LayeredWinImplTraits;

namespace win
{
    template <typename T, typename TWinTraits = LayeredWinImplTraits>
    class LayeredWindow
        : public Window<T, TWinTraits>
    {
    public:
        DECLARE_WND_CLASS(L"LayeredWindow")

        LayeredWindow()
            : alpha_(255) {}

        ~LayeredWindow() {}

        void SetWindowAlpha(BYTE alpha)
        {
            alpha_ = alpha;
        }

        virtual void UpdateRect(HDC hDC, RECT rcUpdate)
        {
            RECT rcWnd;
            ::GetClientRect(m_hWnd, &rcWnd);
            int width = rcWnd.right - rcWnd.left;
            int height = rcWnd.bottom - rcWnd.top;

            HDC hMemDC = ::CreateCompatibleDC(hDC);
            HBITMAP hBitmap = (HBITMAP)::SelectObject(hMemDC, (HBITMAP)image_cache_);
            ::FillRect(hMemDC, &rcUpdate, (HBRUSH)::GetStockObject(BLACK_BRUSH));

            DrawRect(hMemDC, rcUpdate);

            POINT ptDst = { rcWnd.left, rcWnd.top };
            ::ClientToScreen(m_hWnd, &ptDst);
            POINT ptSrc = { 0, 0 };
            SIZE  szWnd = { width, height };

            BLENDFUNCTION blend;
            blend.BlendOp = AC_SRC_OVER;
            blend.BlendFlags = 0;
            blend.SourceConstantAlpha = (BYTE)alpha_;
            blend.AlphaFormat = AC_SRC_ALPHA;
            ::UpdateLayeredWindow(m_hWnd, hDC, &ptDst, &szWnd, hMemDC, &ptSrc, 0, &blend, ULW_ALPHA);

            ::SelectObject(hMemDC, hBitmap);
            ::DeleteDC(hMemDC);
        }

    protected:
        int alpha_;
    };
}

#endif