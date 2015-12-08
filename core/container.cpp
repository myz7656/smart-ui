#include "core/container.h"
#include "core/def.h"
#include "core/controls/span.h"

#include "util/string.h"

namespace core
{
    StContainer::StContainer()
        : first_(0)
        , last_(0) {}

    StContainer::~StContainer() {}

    StObject* StContainer::Clone()
    {
        StContainer* container = new StContainer;
        if (!container)
        {
            return 0;
        }

        CopyTo(container);
        return container;
    }

    void StContainer::Drop()
    {
        Clear();
        if (context_)
        {
            context_->RemovedWidget(this);
        }
        delete this;
    }

    bool StContainer::IsContainer()
    {
        return true;
    }

    StWidget* StContainer::HitTest(int x, int y)
    {
        if (!StWidget::HitTest(x, y))
        {
            return 0;
        }

        StWidget* widget = last_;
        while (widget)
        {
            StWidget* hit = widget->HitTest(x, y);
            if (hit)
            {
                return hit;
            }
            widget = widget->PreviousSibling();
        }

        return this;
    }

    void StContainer::Move(int x, int y, bool onlyChild)
    {
        StWidget* widget = first_;
        while (widget)
        {
            widget->Move(x, y, false);
            widget = widget->NextSibling();
        }

        if (!onlyChild)
        {
            StWidget::Move(x, y, onlyChild);
        }
    }

    void StContainer::SetContext(Context* context)
    {
        StWidget::SetContext(context);
        for (StWidget* widget = first_; widget != 0; widget = widget->NextSibling())
        {
            widget->SetContext(context);
        }
    }

    bool StContainer::IsFilterWidget(const char* name)
    {
        return false;
    }

    bool StContainer::IsScroll()
    {
        return false;
    }


    StWidget* StContainer::FindWidget(const std::string& id)
    {
        if (id.empty())
        {
            return 0;
        }
        KeyWidget key = StringToKeyWidget(id);
        for (StWidget* widget = first_; widget != 0; widget = widget->NextSibling())
        {
            if (widget->Key() == key)
            {
                return widget;
            }

            if (!widget->IsContainer())
            {
                continue;
            }

            StContainer* container = static_cast<StContainer*>(widget);
            StWidget* find = container->FindWidget(key);
            if (find)
            {
                return find;
            }
        }

        return 0;
    }

    StWidget* StContainer::FirstChild()
    {
        return first_;
    }

    StWidget* StContainer::LastChild()
    {
        return last_;
    }

    StWidget* StContainer::FirstChild(const std::string& id)
    {
        if (id.empty())
        {
            return 0;
        }

        KeyWidget key = StringToKeyWidget(id);
        for (StWidget* widget = first_; widget != 0; widget = widget->NextSibling())
        {
            if (widget->Key() == key)
                return widget;
        }

        return 0;
    }

    StWidget* StContainer::LastChild(const std::string& id)
    {
        if (id.empty())
        {
            return 0;
        }
        KeyWidget key = StringToKeyWidget(id);
        for (StWidget* widget = last_; widget != 0; widget = widget->PreviousSibling())
        {
            if (widget->Key() == key)
                return widget;
        }

        return 0;
    }

    void StContainer::LinkBegin(StWidget *add)
    {
        if (!add)
        {
            return;
        }

        add->parent_ = this;

        add->prev_ = 0;
        add->next_ = first_;

        if (first_)
            first_->prev_ = add;
        else
            last_ = add;

        first_ = add;
    }

    void StContainer::LinkEnd(StWidget *add)
    {
        if (!add)
        {
            return;
        }

        add->parent_ = this;

        add->prev_ = last_;
        add->next_ = 0;

        if (last_)
            last_->next_ = add;
        else
            first_ = add;

        last_ = add;
    }

    void StContainer::InsertBefore(StWidget *add, StWidget *before)
    {
        if (add && before && before->parent_ == this)
        {
            add->parent_ = this;

            add->next_ = before;
            add->prev_ = before->prev_;

            if (before->prev_)
            {
                before->prev_->next_ = add;
            }
            else
            {
                first_ = add;
            }

            before->prev_ = add;
        }
    }

    void StContainer::InsertAfter(StWidget *add, StWidget *after)
    {
        if (add && after && after->parent_ == this)
        {
            add->parent_ = this;

            add->next_ = after->next_;
            add->prev_ = after;

            if (after->next_)
            {
                after->next_->prev_ = add;
            }
            else
            {
                last_ = add;
            }

            after->next_ = add;
        }
    }

    void StContainer::Replace(StWidget *with, StWidget *replace)
    {
        if (with && replace && replace->parent_ == this)
        {
            with->parent_ = this;
            with->next_ = replace->next_;
            with->prev_ = replace->prev_;

            if (replace->next_)
                replace->next_->prev_ = with;
            else
                last_ = with;

            if (replace->prev_)
                replace->prev_->next_ = with;
            else
                first_ = with;
        }
    }

    void StContainer::Remove(StWidget *remove)
    {
        if (remove && remove->parent_ == this)
        {
            if (remove->next_)
                remove->next_->prev_ = remove->prev_;
            else
                last_ = remove->prev_;

            if (remove->prev_)
                remove->prev_->next_ = remove->next_;
            else
                first_ = remove->next_;
        }
    }

    StWidget* StContainer::Accept()
    {
        StWidget* widget = first_;

        first_ = 0;
        last_ = 0;

        return widget;
    }

    void StContainer::CopyTo(StContainer* container)
    {
        StWidget::CopyTo(container);
        for (StWidget* widget = first_; widget != 0; widget = widget->NextSibling())
        {
            StObject* obj = widget->Clone();
            if (obj)
            {
                container->LinkEnd(static_cast<StWidget*>(obj));
            }
        }
    }

    void StContainer::Clear()
    {
        StWidget* widget = first_;
        while (widget)
        {
            StWidget* temp = widget->NextSibling();
            if (context_)
            {
                context_->RemovedWidget(temp);
            }
            widget->Drop();
            widget = temp;
        }
        first_ = 0;
        last_ = 0;
    }

    void StContainer::DrawChild(StPainter* painter, RECT* clip)
    {
        for (StWidget* widget = first_; widget != 0; widget = widget->NextSibling())
        {
            widget->Dispatch(ST_PAINT, (WPARAM)painter, (LPARAM)clip);
        }
    }

    LRESULT StContainer::OnLayout(WPARAM wparam, LPARAM lparam, bool* handled)
    {
        if (!StWidget::OnLayout(wparam, lparam, handled))
        {
            return FALSE;
        }

        LayoutParam self;
        self.content = layout_rect_.content;
        self.leaf = false;
        self.row = layout_rect_.content.top;
        self.col = layout_rect_.content.left;
        self.baseline = layout_rect_.content.top;
        self.boundary.left = layout_rect_.content.left;
        self.boundary.top = layout_rect_.content.top;
        self.boundary.right = layout_rect_.content.left;
        self.boundary.bottom = layout_rect_.content.top;
        for (StWidget* iter = first_; iter != 0; iter = iter->next_)
        {
            iter->Dispatch(ST_LAYOUT, (WPARAM)&self, 0);
        }

        ILayout* layoutor = Layoutor();
        if (layoutor)
        {
            layoutor->AfterBlockLayout(*this, self);
        }
        else
        {
            StreamLayout layoutor;
            layoutor.AfterBlockLayout(*this, self);
        }

        return TRUE;
    }

    LRESULT StContainer::OnPaint(WPARAM wparam, LPARAM lparam, bool* handled)
    {
        if (!IsVisible())
        {
            return FALSE;
        }
        StPainter* painter = (StPainter*)wparam;
        RECT* clip = (RECT*)lparam;

        int nclip = BeginPaint(painter, clip);
        if (nclip == 0)
        {
            return FALSE;
        }

        DrawSelf(painter);

        DrawChild(painter, clip);

        EndPaint(painter, nclip);

        return TRUE;
    }
}