#include "win/window.h"
#include "win/layered_window.h"
#include "core/controls/button.h"
#include <atlstr.h>
#include "core/controls/realwnd.h"

class CMainDlg
    : public win::Window<CMainDlg>
    , public core::IMessageObserver
{
protected:

    BEGIN_WIDGET_MESSAGE_MAP()
        WIDGET_LBUTTONUP("button_close", OnClose)
        WIDGET_LBUTTONUP("button_min", OnMin)
        WIDGET_LBUTTONUP("button_change", OnChange)
    END_WIDGET_MESSAGE_MAP()

    BEGIN_MSG_MAP(CMainDlg)
        MESSAGE_HANDLER( WM_CREATE, OnCreate )
        MESSAGE_HANDLER( WM_DESTROY, OnDestroy )
        CHAIN_MSG_MAP(win::Window<CMainDlg>)
    END_MSG_MAP()

    HRESULT OnCreate( UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL bHandled )
    {
        RegisterMessageObserver("button_close", this);
        RegisterMessageObserver("button_min", this);
        RegisterMessageObserver("button_change", this);

        CenterWindow();

        core::StRealWnd* editWnd = (core::StRealWnd*)FindWidget("editwnd");
        if (editWnd)
        {
            m_edit.Create(m_hWnd, NULL, NULL, WS_CLIPCHILDREN | WS_CHILD);
            editWnd->SetHWnd(m_edit.m_hWnd);
        }

        return win::Window<CMainDlg>::OnCreate( uMsg, wParam, lParam, bHandled );
    }

    HRESULT OnDestroy( UINT, WPARAM, LPARAM, BOOL )
    {
        ::PostQuitMessage(0);
        return TRUE;
    }

    void OnClose(core::StWidget* widget, WPARAM wparam, LPARAM lparam)
    {
        DestroyWindow();
    }

    void OnMin(core::StWidget* widget, WPARAM wparam, LPARAM lparam)
    {
    }

    void OnChange(core::StWidget* widget, WPARAM wparam, LPARAM lparam)
    {
        RECT rect;
        ::GetWindowRect(m_hWnd, &rect);
        rect.bottom = rect.top + 345;
        MoveWindow(&rect);

        SetAttribute("progress_div", "visible", "visible");
    }

    void RefreshOnDebug()
    {
        RegisterMessageObserver("button_close", this);
    }

    CEdit m_edit;
};