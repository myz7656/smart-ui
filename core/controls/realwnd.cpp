#include "core/controls/realwnd.h"
#include "win/gdi_painter.h"

namespace core
{
    IMPLY_DYNAMIC_CREATE(StRealWnd, "realwnd")

    StRealWnd::StRealWnd()
        : hwnd_(NULL) {}

    StRealWnd::~StRealWnd() {}

    LRESULT StRealWnd::OnPaint(WPARAM wparam, LPARAM lparam, bool*)
    {
        StPainter* painter = (StPainter*)wparam;
        RECT* clip = (RECT*)lparam;
        if (!painter || !clip)
        {
            return FALSE;
        }

        return TRUE;
    }


    LRESULT StRealWnd::OnLayout(WPARAM wparam, LPARAM lparam, bool* handled)
    {
        if (!StWidget::OnLayout(wparam, lparam, handled))
        {
            return FALSE;
        }

        if (GetHWnd() == NULL)
        {
            return TRUE;
        }

        RECT rc = layout_rect_.content;
        ::MoveWindow(GetHWnd(), rc.left, rc.top, rc.right - rc.left, rc.bottom - rc.top, TRUE);

        return TRUE;
    }

    void StRealWnd::SetHWnd(HWND hWnd)
    {
        hwnd_ = hWnd;
    }

    HWND StRealWnd::GetHWnd()
    {
        return hwnd_;
    }

    LRESULT StRealWnd::OnShow(WPARAM wparam, LPARAM, bool*)
    {
        bool visible = bool(wparam);
        ShowWindow(GetHWnd(), visible ? SW_SHOW : SW_HIDE);
        return TRUE;
    }

    void StRealWnd::Move(int x, int y, bool onlyChild)
    {
        StWidget::Move(x, y, onlyChild);

        if (x != 0 || y != 0)
        {
            RECT rc = layout_rect_.content;
            ::MoveWindow(GetHWnd(), rc.left, rc.top, rc.right - rc.left, rc.bottom - rc.top, TRUE);
        }
    }

}