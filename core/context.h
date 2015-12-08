#ifndef __core_context_h__
#define __core_context_h__

#include <windows.h>

namespace win
{
    class WidgetRoot;
}

namespace core
{
    class StWidget;
    class Context
    {
    public:
        Context()
            : widget_root_(0) {}

        ~Context() {}

        void SetWidgetRoot(win::WidgetRoot* widget_root);

        void SetHWND(HWND hwnd);

        HWND GetHWND();

        void RemovedWidget(StWidget* widget);

    private:
        HWND hwnd_;
        win::WidgetRoot* widget_root_;
    };
}

#endif