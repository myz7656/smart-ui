#include "win/widget_root.h"
#include "util/string.h"


namespace win 
{
    WidgetRoot::WidgetRoot() 
        : body_(0)
        , active_(0)
        , context_(0)
    {
        builder_ = new Builder();
    }

    WidgetRoot::~WidgetRoot()
    {
        if (body_)
        {
            body_->Drop();
            body_ = 0;
            active_ = 0;
        }

        if (builder_)
        {
            delete builder_;
            builder_ = 0;
        }
    }

    BOOL WidgetRoot::ParseXml(LPCTSTR file)
    {
        if (util::String::IsEmpty<wchar_t>(file))
        {
            return false;
        }

        Builder* builder = GetBuilder();
        if (!builder)
        {
            return false;
        }

        if (body_)
        {
            body_->Drop();
            body_ = 0;
        }

        active_ = 0;
        body_ = static_cast<core::StContainer*>(builder->Parse(file));
        body_->SetContext(context_);

        return body_ != 0;
    }

    Builder* WidgetRoot::GetBuilder()
    {
        return builder_;
    }

    core::StWidget* WidgetRoot::Body()
    {
        return body_;
    }

    core::StWidget* WidgetRoot::Active()
    {
        return active_;
    }

    void WidgetRoot::SetActive(core::StWidget* active)
    {
        active_ = active;
    }

    void WidgetRoot::SetRuntimeContext(core::Context* context)
    {
        context_ = context;
        if (context_)
        {
            context_->SetWidgetRoot(this);
        }
    }

    core::StWidget* WidgetRoot::FindWidget(const std::string& id)
    {
        if (body_)
        {
            return body_->FindWidget(id);
        }
        return 0;
    }

    core::Context* WidgetRoot::GetContext()
    {
        return context_;
    }
}