#include "win/window.h"
#include "win/layered_window.h"
#include "core/controls/button.h"
#include <atlstr.h>

class CMainDlg
    : public win::Window<CMainDlg>
    , public core::IMessageObserver
{
protected:

    BEGIN_WIDGET_MESSAGE_MAP()
        WIDGET_LBUTTONUP("button_logout", OnLogout)
        WIDGET_LBUTTONUP("button_setting", OnSetting)
        WIDGET_LBUTTONUP("button_exit", OnExit)
    END_WIDGET_MESSAGE_MAP()

    BEGIN_MSG_MAP(CMainDlg)
        MESSAGE_HANDLER( WM_CREATE, OnCreate )
        MESSAGE_HANDLER( WM_DESTROY, OnDestroy )
        CHAIN_MSG_MAP(win::Window<CMainDlg>)
    END_MSG_MAP()

    HRESULT OnCreate( UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL bHandled )
    {
        RegisterMessageObserver("button_logout", this);
        RegisterMessageObserver("button_setting", this);
        RegisterMessageObserver("button_exit", this);
        RegisterMessageObserver("my_disk", this);
        RegisterMessageObserver("project_disk", this);
        RegisterMessageObserver("group_disk", this);
        RegisterMessageObserver("website", this);

        return win::Window<CMainDlg>::OnCreate( uMsg, wParam, lParam, bHandled );
    }

    HRESULT OnDestroy( UINT, WPARAM, LPARAM, BOOL )
    {
        ::PostQuitMessage(0);
        return TRUE;
    }

    void OnLogout(core::StWidget* widget, WPARAM wparam, LPARAM lparam)
    {
        
    }

    void OnSetting(core::StWidget* widget, WPARAM wparam, LPARAM lparam)
    {
    }

    void OnExit(core::StWidget* widget, WPARAM wparam, LPARAM lparam)
    {
        DestroyWindow();

    }

    void RefreshOnDebug()
    {
        RegisterMessageObserver("button_close", this);
    }
};