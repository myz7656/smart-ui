#ifndef __win_tooltip_h__
#define __win_tooltip_h__

#include <windows.h>

namespace win
{
    class ToolTip
    {
    public:
        ToolTip()
            : tip_wnd_(NULL)
        {
            memset(&tool_info_, 0, sizeof(TOOLINFO));
            tool_info_.cbSize = sizeof(TOOLINFO);
        }

        BOOL Create(HWND hParent)
        {
            tip_wnd_ = ::CreateWindowEx(WS_EX_TOPMOST, TOOLTIPS_CLASS, NULL,
                WS_POPUP | TTS_NOPREFIX | TTS_ALWAYSTIP, CW_USEDEFAULT, CW_USEDEFAULT,
                CW_USEDEFAULT, CW_USEDEFAULT, hParent, NULL, NULL, NULL);

            tool_info_.hwnd = ::GetParent(tip_wnd_);
            tool_info_.uFlags = TTF_IDISHWND;
            tool_info_.uId = (UINT_PTR)tool_info_.hwnd;

            ::SendMessage(tip_wnd_, TTM_ADDTOOL, 0, (LPARAM)&tool_info_);

            return TRUE;
        }

        int SetMaxTipWidth(int iWidth)
        {
            return (int) ::SendMessage(tip_wnd_, TTM_SETMAXTIPWIDTH, 0, iWidth);
        }

        void Activate(BOOL bActivate)
        {
            ::SendMessage(tip_wnd_, TTM_ACTIVATE, bActivate, 0L);
        }

        BOOL IsWindow()
        {
            return ::IsWindow(tip_wnd_);
        }

        void RelayEvent(LPMSG lpMsg)
        {
            ::SendMessage(tip_wnd_, TTM_RELAYEVENT, 0, (LPARAM)lpMsg);
        }

        void UpdateTipText(LPCTSTR lpszText)
        {
            std::wstring text;
            if (lpszText)
            {
                text = lpszText;
            }

            if (text != str_old_tip_)
            {
                str_old_tip_ = text;

                tool_info_.lpszText = (LPTSTR)text.c_str();
                ::SendMessage(tip_wnd_, TTM_UPDATETIPTEXT, 0, (LPARAM)&tool_info_);
            }
        }

    private:
        std::wstring str_old_tip_;
        HWND        tip_wnd_;
        TOOLINFO    tool_info_;
    };

}

#endif