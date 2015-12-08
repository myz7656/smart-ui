#include "core/context.h"

#include "win/widget_root.h"

namespace core
{
    void Context::SetWidgetRoot(win::WidgetRoot* widget_root)
    {
        widget_root_ = widget_root;
    }

    void Context::SetHWND(HWND hwnd)
    {
        hwnd_ = hwnd;
    }

    HWND Context::GetHWND()
    {
        return hwnd_;
    }

    void Context::RemovedWidget(StWidget* widget)
    {
        if (widget_root_)
        {
            StWidget* active = widget_root_->Active();
            if (active == widget)
            {
                widget_root_->SetActive(0);
            }
        }
    }
}