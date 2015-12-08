#ifndef __win_window_h__
#define __win_window_h__

#include <windows.h>
#include <atlbase.h>
#include <atlwin.h>

#include "win/widget_root.h"
#include "win/image.h"
#include "win/gdi_painter.h"

#include "third_party/inc/wtl/atlapp.h"
#include "third_party/inc/wtl/atlctrls.h"

#define BEGIN_WIDGET_MESSAGE_MAP()\
    virtual bool OnMessageObserver(core::StWidget* widget, UINT umsg, WPARAM wparam, LPARAM lparam, bool before)\
    {\
        bool handled = false;\
        std::string widget_id;\
        widget->GetAttribute(core::WIDGET_PROPERTY_ID, &widget_id);\
        if (widget_id.empty())\
        {\
            return false;\
        }

#define END_WIDGET_MESSAGE_MAP()\
            handled = false;\
        return handled;\
    }

#define WIDGET_MESSAGE_HANDLE(msg, id, order, func)\
        if (before == order && umsg == msg && strcmp(widget_id.c_str(), id) == 0)\
        {\
            func(widget, wparam, lparam);\
            handled = true;\
        }\
        else

#define CHAIN_WIDGET_MESSAGE_MAP(theclass)\
        if (theclass::OnMessageObserver(widget, umsg, wparam, lparam, before))\
        {\
            handled = true;\
        }\
        else

#define WIDGET_LBUTTONUP(id, func)  WIDGET_MESSAGE_HANDLE(core::ST_LBUTTONUP,  id, false, func)
#define WIDGET_MOUSEOVER(id, func)  WIDGET_MESSAGE_HANDLE(core::ST_MOUSEHOVER, id, false, func)
#define WIDGET_MOUSELEAVE(id, func) WIDGET_MESSAGE_HANDLE(core::ST_MOUSELEAVE, id, false, func)


typedef CWinTraits<WS_POPUP | WS_VISIBLE | WS_CLIPCHILDREN | WS_SYSMENU | WS_MINIMIZEBOX | WS_MAXIMIZEBOX | WS_TABSTOP>    WinImplTraits;



namespace win
{
    template <typename T, typename TWinTraits = WinImplTraits>
    class Window
        : public CWindowImpl<T, CWindow, TWinTraits>
    {
    public:
        DECLARE_WND_CLASS(L"Window")

        Window()
            : alpha_(255)
            , capture_(false)
            , track_flag_(FALSE)
            , ret_code_(0)
            , exit_modal_loop_(FALSE)
        {
            widget_root_.SetRuntimeContext(&context_);
        }

        ~Window() {}

        void SetWindowAlpha(int alpha)
        {
            alpha_ = alpha;
        }

        BOOL Parse(const wchar_t* file)
        {
            if (!file)
            {
                return FALSE;
            }
            xml_file_ = file;
            if (widget_root_.ParseXml(file))
            {
                return TRUE;
            }

            return FALSE;
        }

        Builder* GetBuilder()
        {
            return widget_root_.GetBuilder();
        }

        core::StWidget* FindWidget(const std::string& id)
        {
            return widget_root_.FindWidget(id);
        }

        BOOL RegisterMessageObserver(const std::string &id, core::IMessageObserver* observer)
        {
            if (id.size() <= 0 || !observer)
            {
                return FALSE;
            }

            core::StWidget * widget = widget_root_.FindWidget(id);
            if (!widget)
            {
                return FALSE;
            }

            widget->AddMessageObserver(observer);
            return TRUE;
        }

        BOOL SetAttribute(const std::string &id, const std::string &name, const std::string &value)
        {
            if (id.empty() || name.empty())
            {
                return FALSE;
            }

            core::StWidget *widget = widget_root_.FindWidget(id);
            if (!widget)
            {
                return FALSE;
            }

            DWORD result = 0;
            if (!widget->SetAttribute(name.c_str(), value.c_str(), &result))
            {
                return FALSE;
            }

            if ((result & core::ATTRIBUTE_TRAIT_LAYOUT) == core::ATTRIBUTE_TRAIT_LAYOUT)
            {
                widget->UpdateLayout();
            }
            else if ((result & core::ATTRIBUTE_TRAIT_PAINT) == core::ATTRIBUTE_TRAIT_PAINT)
            {
                widget->Invalidate();
            }

            return TRUE;
        }

        void BringWinToForground()
        {
            HWND hCurWnd = ::GetForegroundWindow();
            DWORD dwMyID = ::GetCurrentThreadId();
            DWORD dwCurID = ::GetWindowThreadProcessId(hCurWnd, NULL);
            ::AttachThreadInput(dwCurID, dwMyID, TRUE);
            ::SetWindowPos(m_hWnd, HWND_TOPMOST, 0, 0, 0, 0, SWP_NOSIZE | SWP_NOMOVE);
            ::SetWindowPos(m_hWnd, HWND_NOTOPMOST, 0, 0, 0, 0, SWP_NOSIZE | SWP_NOMOVE);
            ::SetForegroundWindow(m_hWnd);
            ::AttachThreadInput(dwCurID, dwMyID, FALSE);
            ::SetFocus(m_hWnd);
            ::SetActiveWindow(m_hWnd);
        }

        INT_PTR DoModal(HWND hWndParent = NULL, LPRECT rect = NULL)
        {
            BOOL bEnableParent = FALSE;

            if (NULL == hWndParent)
            {
                hWndParent = ::GetActiveWindow();
            }

            if (hWndParent && hWndParent != ::GetDesktopWindow() && ::IsWindowEnabled(hWndParent))
            {
                ::EnableWindow(hWndParent, FALSE);
                bEnableParent = TRUE;
            }

            HWND hWnd = Create(hWndParent, rect);
            if (!hWnd)
            {
                ::EnableWindow(hWndParent, TRUE);
                return 0;
            }

            CenterWindow(hWndParent);

            DWORD dwForeID = ::GetWindowThreadProcessId(::GetForegroundWindow(), NULL);
            DWORD dwCurID = ::GetCurrentThreadId();

            SystemParametersInfo(SPI_SETFOREGROUNDLOCKTIMEOUT, 0, (LPVOID)0, 0);

            ::SetWindowPos(hWnd, HWND_TOP, 0, 0, 0, 0, SWP_NOSIZE | SWP_NOMOVE | SWP_SHOWWINDOW);

            if (dwForeID != dwCurID)
            {
                ::AttachThreadInput(dwCurID, dwForeID, TRUE);
            }

            ::SetForegroundWindow(hWnd);

            if (dwForeID != dwCurID)
            {
                ::AttachThreadInput(dwCurID, dwForeID, FALSE);
            }

            RunModalLoop();

            exit_modal_loop_ = FALSE;
            SetWindowPos(
                NULL, 0, 0, 0, 0, 
                SWP_HIDEWINDOW | SWP_NOSIZE | SWP_NOMOVE | SWP_NOACTIVATE | SWP_NOZORDER);

            if (bEnableParent)
            {
                ::EnableWindow(hWndParent, TRUE);

                if (hWndParent != NULL && ::GetActiveWindow() == m_hWnd)
                    ::SetActiveWindow(hWndParent);
            }

            DestroyWindow();
            return ret_code_;
        }

        void EndDialog(UINT uResult)
        {
            ret_code_ = uResult;
            exit_modal_loop_ = TRUE;
            PostMessage(WM_NULL);
        }

    protected:
        BEGIN_MSG_MAP(Window)
            MESSAGE_RANGE_HANDLER(WM_MOUSEFIRST, WM_MOUSELAST, OnMouseMessage)
            MESSAGE_HANDLER(WM_CREATE, OnCreate)
            MESSAGE_HANDLER(WM_SIZE, OnSize)
            MESSAGE_HANDLER(WM_PAINT, OnPaint)
            MESSAGE_HANDLER(WM_ERASEBKGND, OnEraseBkgnd)
            MESSAGE_HANDLER(WM_LBUTTONDOWN, OnLButtonDown)
            MESSAGE_HANDLER(WM_LBUTTONUP, OnLButtonUp)
            MESSAGE_HANDLER(WM_MOUSEMOVE, OnMouseMove)
            MESSAGE_HANDLER(WM_MOUSELEAVE, OnMouseLeave)
            MESSAGE_HANDLER(WM_MOUSEWHEEL, OnMouseWheel)
            MESSAGE_HANDLER(WM_KEYDOWN, OnKeyDown)
        END_MSG_MAP()

        HRESULT OnCreate(UINT, WPARAM, LPARAM, BOOL)
        {
            context_.SetHWND(m_hWnd);

            m_ctrlTip.Create(m_hWnd, NULL, NULL, 0, WS_EX_TOPMOST);
            m_ctrlTip.AddTool(m_hWnd);
            m_ctrlTip.SetMaxTipWidth(260);
            m_ctrlTip.Activate(TRUE);
            return TRUE;
        }

        HRESULT OnSize(UINT, WPARAM wParam, LPARAM lParam, BOOL)
        {
            switch(wParam)
            {
            case SIZE_MINIMIZED:
            case SIZE_MAXHIDE:
                return TRUE;

            case SIZE_MAXIMIZED:
            case SIZE_MAXSHOW:
            case SIZE_RESTORED:
                break;
            }

            int width = LOWORD(lParam);
            int height = HIWORD(lParam);
            if (width == 0 || height == 0)
            {
                return TRUE;
            }

            if ((HBITMAP)image_cache_)
            {
                image_cache_.Release();
            }

            image_cache_.CreateDIBSection(width, height);
            core::StWidget *body = widget_root_.Body();
            if (body)
            {
                RECT rcWnd = {0, 0, width, height};

                core::LayoutParam layout_param;
                layout_param.leaf = false;
                layout_param.content = rcWnd;

                body->Dispatch(core::ST_LAYOUT, (WPARAM)&layout_param, 0);
            }

            HDC hDC = ::GetDC(m_hWnd);
            RECT rcUpdate = { 0, 0, width, height };
            UpdateRect(hDC, rcUpdate);
            ::ReleaseDC(m_hWnd, hDC);

            return TRUE;
        }

        HRESULT OnPaint(UINT, WPARAM, LPARAM, BOOL)
        {
            RECT rcUpdate;
            ::GetUpdateRect(m_hWnd, &rcUpdate, FALSE);

            PAINTSTRUCT ps;
            HDC hDC = ::BeginPaint(m_hWnd, &ps);

            UpdateRect(hDC, rcUpdate);

            ::EndPaint(m_hWnd, &ps);

            return TRUE;
        }

        HRESULT OnEraseBkgnd(UINT, WPARAM, LPARAM, BOOL)
        {
            return TRUE;
        }

        HRESULT OnLButtonDown(UINT, WPARAM wParam, LPARAM lParam, BOOL)
        {
            int x = GET_X_LPARAM(lParam);
            int y = GET_Y_LPARAM(lParam);

            core::StWidget* body = widget_root_.Body();
            core::StWidget* widget_down = 0;

            if (body)
            {
                widget_down = body->HitTest(x, y);
            }

            if (widget_down)
            {
                widget_down->Dispatch(core::ST_LBUTTONDOWN, (WPARAM)wParam, (LPARAM)lParam);
                if (widget_down->IsDraggable())
                {
                    ::SendMessage(m_hWnd, WM_SYSCOMMAND, SC_MOVE | HTCAPTION, 0);
                    ::SendMessage(m_hWnd, WM_LBUTTONUP, wParam, lParam);
                }
                else if (widget_down->IsCapture())
                {
                    capture_ = true;
                    ::SetCapture(m_hWnd);
                }
            }
            return TRUE;
        }

        HRESULT OnLButtonUp(UINT, WPARAM wParam, LPARAM lParam, BOOL)
        {
            if (capture_)
            {
                core::StWidget* active = widget_root_.Active();
                if (active)
                {
                    active->Dispatch(core::ST_LBUTTONUP, (WPARAM)wParam, (LPARAM)lParam);
                }

                ::ReleaseCapture();
                capture_ = false;
                return TRUE;
            }

            int x = GET_X_LPARAM(lParam);
            int y = GET_Y_LPARAM(lParam);

            core::StWidget* body = widget_root_.Body();
            core::StWidget* widget_up = 0;

            if (body)
            {
                widget_up = body->HitTest(x, y);
            }

            if (widget_up)
            {
                widget_up->Dispatch(core::ST_LBUTTONUP, (WPARAM)wParam, (LPARAM)lParam);
            }

            return TRUE;
        }

        HRESULT OnMouseMessage(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
        {
            MSG msg = { m_hWnd, uMsg, wParam, lParam };

            if (m_ctrlTip.IsWindow())
            {
                m_ctrlTip.RelayEvent(&msg);
            }

            bHandled = FALSE; 
            return FALSE;
        }

        HRESULT OnMouseMove(UINT, WPARAM wParam, LPARAM lParam, BOOL)
        {
            if (!track_flag_)
            {
                TRACKMOUSEEVENT tme;
                tme.cbSize = sizeof(tme);
                tme.hwndTrack = m_hWnd;
                tme.dwFlags = TME_LEAVE;
                tme.dwHoverTime = 0;
                track_flag_ = _TrackMouseEvent(&tme);
            }

            if (capture_)
            {
                core::StWidget* active = widget_root_.Active();
                if (active)
                {
                    active->Dispatch(core::ST_MOUSEHOVER, (WPARAM)wParam, (LPARAM)lParam);
                }
                return TRUE;
            }

            int x = GET_X_LPARAM(lParam);
            int y = GET_Y_LPARAM(lParam);
            core::StWidget* active = widget_root_.Active();
            core::StWidget* body = widget_root_.Body();
            core::StWidget* widget = 0;

            if (body)
            {
                widget = body->HitTest(x, y);
            }

            if (widget && widget != active)
            {
                widget->Dispatch(core::ST_MOUSEHOVER, (WPARAM)wParam, (LPARAM)lParam);
                std::wstring strTitle = widget->Title();

                if (strTitle != m_strOldTip)
                {
                    m_ctrlTip.UpdateTipText(strTitle.c_str(), m_hWnd);
                    m_strOldTip = strTitle;
                }

                if (active)
                {
                    active->Dispatch(core::ST_MOUSELEAVE, (WPARAM)wParam, (LPARAM)lParam);
                }
                widget_root_.SetActive(widget);
            }
            else if (widget == NULL)
            {
                m_ctrlTip.UpdateTipText(_T(""), m_hWnd);
            }
            return TRUE;
        }

        HRESULT OnMouseLeave(UINT, WPARAM wParam, LPARAM lParam, BOOL)
        {
            track_flag_ = false;
            core::StWidget* active = widget_root_.Active();
            if (active)
            {
                active->Dispatch(core::ST_MOUSELEAVE, wParam, lParam);
                widget_root_.SetActive(NULL);
            }

            return S_OK;
        }

        HRESULT OnMouseWheel(UINT, WPARAM wParam, LPARAM lParam, BOOL)
        {
            core::StWidget * active = widget_root_.Active();
            if (active)
            {
                core::StWidget* parent = active->Parent();
                while (parent)
                {
                    if (parent->IsContainer() && static_cast<core::StContainer*>(parent)->IsScroll())
                    {
                        break;
                    }
                    parent = parent->Parent();
                }
                if (parent)
                {
                    parent->Dispatch(core::ST_MOUSEWHEEL, (WPARAM)wParam, (LPARAM)lParam);
                }
                else
                {
                    active->Dispatch(core::ST_MOUSEWHEEL, (WPARAM)wParam, (LPARAM)lParam);
                }
            }
            return TRUE;
        }

        HRESULT OnKeyDown( UINT, WPARAM wParam, LPARAM lParam, BOOL )
        {
            if (wParam == VK_F5)
            {
                if (widget_root_.ParseXml(xml_file_.c_str()))
                {
                    RECT rect;
                    ::GetWindowRect(m_hWnd, &rect);
                    int width = rect.right - rect.left;
                    int height = rect.bottom - rect.top;
                    LPARAM sizeParam = MAKELONG(width, height);
                    OnSize(0, SIZE_RESTORED, sizeParam, TRUE);
                    ::InvalidateRect(m_hWnd, &rect, TRUE);
                    RefreshOnDebug();
                }
            }

            return TRUE;
        }

        void UpdateRect(HDC hDC, RECT rcUpdate)
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
            POINT ptSrc = { 0, 0 };
            SIZE  szWnd = { width, height };

            DWORD dwStyleEx = GetWindowLong(GWL_EXSTYLE);
            if (dwStyleEx & WS_EX_LAYERED)
            {
                ::ClientToScreen(m_hWnd, &ptDst);

                BLENDFUNCTION blend;
                blend.BlendOp = AC_SRC_OVER;
                blend.BlendFlags = 0;
                blend.SourceConstantAlpha = (BYTE)alpha_;
                blend.AlphaFormat = AC_SRC_ALPHA;
                ::UpdateLayeredWindow(m_hWnd, hDC, &ptDst, &szWnd, hMemDC, &ptSrc, 0, &blend, ULW_ALPHA);
            }
            else
            {
                ::BitBlt(hDC, ptDst.x, ptDst.y, szWnd.cx, szWnd.cy, hMemDC, ptSrc.x, ptSrc.y, SRCCOPY);
            }

            ::SelectObject(hMemDC, hBitmap);
            ::DeleteDC(hMemDC);
        }

        BOOL DrawRect(HDC hDC, RECT rcUpdate)
        {
            core::StWidget *body = widget_root_.Body();
            if (!body)
            {
                return FALSE;
            }

            RECT rect = {rcUpdate.left, rcUpdate.top, rcUpdate.right, rcUpdate.bottom};

            HRGN rgnUpdate = ::CreateRectRgn(rcUpdate.left, rcUpdate.top, rcUpdate.right, rcUpdate.bottom);
            if (rgnUpdate)
            {
                ::SelectClipRgn(hDC, rgnUpdate);
            }

            HDCCanvas canvas;
            canvas.Attatch(hDC);

            BOOL bLayered = GetExStyle() & WS_EX_LAYERED;
            WinPainter painter(&canvas, bLayered);
            body->Dispatch(core::ST_PAINT, (WPARAM)&painter, (LPARAM)&rect);

            if (rgnUpdate)
            {
                ::DeleteObject(rgnUpdate);
            }

            return TRUE;
        }

        virtual void RefreshOnDebug() {}

        void RunModalLoop()
        {
            BOOL bRet;
            MSG msg;

            for(;;)
            {
                if (::PeekMessage(&msg, NULL, 0, 0, PM_NOREMOVE))
                {
                    if (WM_QUIT == msg.message)
                        break;
                }

                if (exit_modal_loop_ || NULL == m_hWnd || !::IsWindow(m_hWnd))
                    break;

                bRet = ::GetMessage(&msg, NULL, 0, 0);

                if (bRet == -1)
                {
                    continue;   // error, don't process
                }
                else if (!bRet)
                {
                    ATLTRACE(L"Why Receive WM_QUIT here?\r\n");
                    break;   // WM_QUIT, exit message loop
                }

                if (!IsDialogMessage(&msg))
                {
                    ::TranslateMessage(&msg);
                    ::DispatchMessage(&msg);
                }
            }
        }
    protected:
        core::Context   context_;

        WidgetRoot      widget_root_;
        Image           image_cache_;

        int             alpha_;
        std::wstring    xml_file_;

        bool            capture_;
        CToolTipCtrl    m_ctrlTip;
        std::wstring    m_strOldTip;
        BOOL            track_flag_;
        UINT            ret_code_;
        BOOL            exit_modal_loop_;
    };
}

#endif