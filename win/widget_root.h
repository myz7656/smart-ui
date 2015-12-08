#ifndef __win_widget_root_h__
#define __win_widget_root_h__

#include "core/widget.h"
#include "core/container.h"
#include "core/style.h"
#include "win/builder.h"

namespace win 
{
    class WidgetRoot
    {
    public:
        WidgetRoot();
        ~WidgetRoot();

        BOOL ParseXml(LPCTSTR file);

        Builder* GetBuilder();
        core::StWidget* Body();
        core::StWidget* Active();
        void SetActive(core::StWidget* active);

        void SetRuntimeContext(core::Context* context);

        core::StWidget* FindWidget(const std::string& id);
        core::Context* GetContext();

    private:
        core::StContainer* body_;
        core::StWidget*    active_;
        Builder*           builder_;
        core::Context*     context_;

    private:
        DISABLE_COPY_AND_ASSIGN(WidgetRoot)
    };
}

#endif