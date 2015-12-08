#include "win/window.h"
#include "win/layered_window.h"
#include "core/controls/button.h"
#include "base/time_ticks.h"
#include <atlstr.h>

#include "animate/animate_container.h"
#include "animate/frame_animate.h"


class CMenuDlg
    : public win::Window<CMenuDlg>
    , public core::IMessageObserver
    , public animate::AttributeAnimate::Delegate
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
        CHAIN_MSG_MAP(win::Window<CMenuDlg>)
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

        return win::Window<CMenuDlg>::OnCreate( uMsg, wParam, lParam, bHandled );
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
        animate::AttributeFrameAnimate* animate = new animate::AttributeFrameAnimate;
        animate->SetId("user_icon");
        animate->SetAttribute("frame-index");
        animate->SetCount(3);
        animate->SetInterval(100);
        animate->SetTotal(INFINITE);
        animate->SetDelegate(this);

        container_.Start(animate);
    }

    void OnExit(core::StWidget* widget, WPARAM wparam, LPARAM lparam)
    {
        DestroyWindow();

    }

    void RefreshOnDebug()
    {
        RegisterMessageObserver("button_close", this);
    }

    virtual void OnAttributeValueChanged(std::string id, std::string attribute, std::string value)
    {
        static __int64 last = base::TimeTicks::Now(), count = 0;
        count++;
        //if (count % 10 == 0)
        {
            __int64 now = base::TimeTicks::Now();
            __int64 off = now - last;
            TCHAR str_value[16] = {0};
            _i64tow_s(off / count, str_value, 16, 10);
            ::OutputDebugString(L"OnAttributeValueChanged---");
            ::OutputDebugString(str_value);
            ::OutputDebugString(L"\n");
        }
        if (id == "user_icon")
        {
            SetAttribute(id, attribute, value);
        }
    }

private:
    animate::AnimateContainer container_;
};