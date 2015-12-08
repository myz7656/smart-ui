#include "core/controls/list.h"
#include "core/controls/break.h"

namespace core
{
    const char * LIST_WIDGET_PROPERTY_SCROLL_BACKGROUND   = "scroll-background";
    const char * LIST_WIDGET_PROPERTY_SCROLL_BORDER       = "scroll-border";
    const char * LIST_WIDGET_PROPERTY_SCROLL_WIDTH        = "scroll-width";



    IMPLY_DYNAMIC_CREATE(StList, "list")



    BEGIN_ATTRIBUTE_MAP(StList)
        ATTRIBUTE(LIST_WIDGET_PROPERTY_SCROLL_BACKGROUND, StAttributeColor, ATTRIBUTE_TRAIT_PAINT)
        ATTRIBUTE(LIST_WIDGET_PROPERTY_SCROLL_BORDER, StAttributeColor, ATTRIBUTE_TRAIT_PAINT)
        ATTRIBUTE(LIST_WIDGET_PROPERTY_SCROLL_WIDTH, StAttributePixel, ATTRIBUTE_TRAIT_BOTH)
        CHAIN_ATTRIBUTE_MAP(StContainer)
    END_ATTRIBUTE_MAP()

    StList::StList()
        : adapter_(0) {}

    StList::~StList() {}

    StWidget* StList::HitTest(int x, int y)
    {
        if (!StWidget::HitTest(x, y))
        {
            return 0;
        }

        if (IsPtInScroll(x, y))
        {
            return this;
        }

        return StContainer::HitTest(x, y);
    }

    bool StList::IsScroll()
    {
        return true;
    }


    core::Color StList::ScrollBackground()
    {
        core::Color color;
        GetAttribute(LIST_WIDGET_PROPERTY_SCROLL_BACKGROUND, &color);
        return color;
    }

    core::Color StList::ScrollBorder()
    {
        core::Color color;
        GetAttribute(LIST_WIDGET_PROPERTY_SCROLL_BORDER, &color);
        return color;
    }

    int StList::ScrollWidth()
    {
        int width = 0;
        GetAttribute(LIST_WIDGET_PROPERTY_SCROLL_WIDTH, &width, (int)DEFAULT_BAR_LENGTH);
        return width;
    }

    void StList::SetScrollBackground(core::Color color)
    {
        AddAttribute(LIST_WIDGET_PROPERTY_SCROLL_BACKGROUND, color);
    }

    void StList::SetScrollBorder(core::Color color)
    {
        AddAttribute(LIST_WIDGET_PROPERTY_SCROLL_BORDER, color);
    }

    void StList::SetScrollWidth(int width)
    {
        AddAttribute(LIST_WIDGET_PROPERTY_SCROLL_WIDTH, width);
    }


    void StList::SetAdapter(Adapter* adapter)
    {
        recycler_.Clear();
        adapter_ = adapter;
    }

    void StList::NotifyDataSetChanged()
    {
        if (adapter_)
        {
            int total_height = GetTotalHeight();
            int real_height = GetRealHeight();
            scroll_context_.scroll_pos = min(scroll_context_.scroll_pos, total_height - real_height);
            scroll_context_.scroll_pos = max(0, scroll_context_.scroll_pos);

            Adjust(true);

            UpdateScrollBarSizeAndPosition();

            Invalidate();
        }
    }

    LRESULT StList::OnLayout(WPARAM wparam, LPARAM lparam, bool* handled)
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

        for (StWidget* iter = FirstChild(); iter != 0; iter = iter->NextSibling())
        {
            iter->Dispatch(ST_LAYOUT, (WPARAM)&self, 0);
            self.row = self.baseline;
            self.col = layout_rect_.content.left;
        }

        return TRUE;
    }

    LRESULT StList::OnPaint(WPARAM wparam, LPARAM lparam, bool* handled)
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

        DrawScroll(painter);

        EndPaint(painter, nclip);

        return TRUE;
    }

    LRESULT StList::OnMouseHover(WPARAM wparam, LPARAM lparam, bool* handled)
    {
        if (IsCapture())
        {
            RECT content = ContentRect();
            int y = ((int)(short)HIWORD(lparam));

            y = max(y, content.top);
            y = min(y, content.bottom);

            int offset = y - scroll_context_.cursor_pos;
            scroll_context_.cursor_pos = y;
            if (offset != 0)
            {
                int height = GetRealHeight();
                int total_height = GetTotalHeight();

                int last_bar_pos_y = scroll_context_.bar_pos;

                scroll_context_.bar_pos += offset;
                scroll_context_.bar_pos = max(0, scroll_context_.bar_pos);
                scroll_context_.bar_pos = min(scroll_context_.bar_pos, height - scroll_context_.bar_length);

                int bar_range = height - scroll_context_.bar_length;
                int scroll_range = total_height - height;

                double position = (double)scroll_context_.bar_pos / (double)bar_range * (double)scroll_range;

                SetScrollViewPosition((int)position);
            }
            return TRUE;
        }

        return StContainer::OnMouseHover(wparam, lparam, handled);
    }

    LRESULT StList::OnMouseLeave(WPARAM wparam, LPARAM lparam, bool* handled)
    {
        return StContainer::OnMouseLeave(wparam, lparam, handled);
    }

    LRESULT StList::OnLButtonDown(WPARAM wparam, LPARAM lparam, bool* handled)
    {
        int x = ((int)(short)LOWORD(lparam));
        int y = ((int)(short)HIWORD(lparam));

        bool capture = false;
        if (IsPtInScroll(x, y))
        {
            capture = true;
        }

        if (capture)
        {
            SetCapture(true);
            scroll_context_.cursor_pos = y;
            return TRUE;
        }

        return StContainer::OnLButtonDown(wparam, lparam, handled);
    }

    LRESULT StList::OnLButtonUp(WPARAM wparam, LPARAM lparam, bool* handled)
    {
        if (IsCapture())
        {
            SetCapture(false);
            scroll_context_.cursor_pos = 0;
            return TRUE;
        }

        return StContainer::OnLButtonUp(wparam, lparam, handled);
    }

    LRESULT StList::OnMouseWheel(WPARAM wparam, LPARAM lparam, bool* handled)
    {
        int total_height = GetTotalHeight();
        int real_height = GetRealHeight();
        if (total_height <= real_height)
        {
            return StContainer::OnMouseWheel(wparam, lparam, handled);
        }

        int delta = (int)(short)HIWORD(wparam);

        int last = scroll_context_.scroll_pos;
        last -= delta;

        SetScrollViewPosition(last);

        UpdateScrollBarSizeAndPosition();

        Invalidate();

        return TRUE;
    }

    int StList::GetTotalHeight()
    {
        int total = 0;
        if (adapter_)
        {
//             int count = adapter_->GetCount();
//             for (int i = 0; i < count; i++)
//             {
//                 total += adapter_->GetHeight(i);
//             }

            total = adapter_->GetTotalHeight();
        }
        return total;
    }

    int StList::GetRealHeight()
    {
        return layout_rect_.content.bottom - layout_rect_.content.top;
    }

    RECT StList::GetScrollRect()
    {
        int width = ScrollWidth();
        RECT rect;
        rect.top = layout_rect_.content.top + scroll_context_.bar_pos;
        rect.bottom = rect.top + scroll_context_.bar_length;
        rect.right = layout_rect_.content.right;
        rect.left = rect.right - width;

        return rect;
    }

    bool StList::IsPtInScroll(int x, int y)
    {
        RECT rect = GetScrollRect();

        POINT pt = {x, y};
        if (::PtInRect(&rect, pt))
        {
            return true;
        }
        return false;
    }

    void StList::UpdateScrollBarSizeAndPosition()
    {
        scroll_context_.bar_pos = 0;
        scroll_context_.bar_length = 0;

        int height = GetRealHeight();

        int total_height = GetTotalHeight();

        if (total_height > height)
        {
            double size_y = (double)height / (double)total_height * (double)height;
            scroll_context_.bar_length = (int)size_y;
            scroll_context_.bar_length = max(scroll_context_.bar_length, DEFAULT_BAR_LENGTH);

            int scroll_range = total_height - height;
            int bar_range = height - scroll_context_.bar_length;

            double bar_pos = (double)scroll_context_.scroll_pos / (double)scroll_range * (double)bar_range;
            scroll_context_.bar_pos = (int)bar_pos;
        }
    }

    void StList::DrawScroll(StPainter* painter)
    {
        core::Color background = ScrollBackground();
        core::Color border = ScrollBorder();

        RECT rect = GetScrollRect();

        painter->DrawBackground(rect, background, DEFAULT_BAR_ALPHA);
        painter->DrawBorder(rect, BORDER_STYLE_SOLID, DEFAULT_BAR_BORDER_WIDTH, border, DEFAULT_BAR_ALPHA);
    }

    void StList::SetScrollViewPosition(int position)
    {
        int height = GetRealHeight();
        int total_height = GetTotalHeight();

        int offset = scroll_context_.scroll_pos;

        scroll_context_.scroll_pos = position;
        scroll_context_.scroll_pos = max(0, scroll_context_.scroll_pos);
        scroll_context_.scroll_pos = min(scroll_context_.scroll_pos, total_height - height);

        offset -= scroll_context_.scroll_pos;

        if (offset != 0)
        {
            bool upward = (offset < 0);
            Adjust(upward);
        }
    }

    void StList::Adjust(bool upward)
    {
        Accept();
        AdapterContext context = GetAdapterContext();

        LayoutParam self;
        self.content = layout_rect_.content;
        self.leaf = false;
        self.row = layout_rect_.content.top + context.offset;
        self.col = layout_rect_.content.left;
        self.baseline = layout_rect_.content.top + context.offset;

        Recycler recycle;

        for (int index = context.head; index < context.tail; ++index)
        {
            int id = adapter_->GetId(index);
            Recycler::Cluster cluster = recycler_.Recycle(id, true, upward);
            if (cluster.IsOK())
            {
                StWidget* widget = cluster.widget;
                int type = adapter_->GetType(index);
                int height = 0;
                if (type == cluster.type)
                {
                    height = cluster.height;
                    widget->MoveTo(self.col, self.row);
                }
                else
                {
                    widget = adapter_->GetWidget(index, widget);
                    widget->Dispatch(ST_LAYOUT, (WPARAM)&self, 0);
                    height = adapter_->GetHeight(index);
                }
                self.row += height;
                self.col = layout_rect_.content.left;
                widget->SetContext(context_);
                LinkEnd(widget);
                recycle.Save(id, type, height, widget);
            }
            else
            {
                cluster = recycler_.Recycle(id, false, upward);
                StWidget* widget = cluster.widget;
                widget = adapter_->GetWidget(index, widget);
                if (widget)
                {
                    widget->Dispatch(ST_LAYOUT, (WPARAM)&self, 0);
                    int height = adapter_->GetHeight(index);
                    int type = adapter_->GetType(index);
                    self.row += height;
                    self.col = layout_rect_.content.left;

                    widget->SetContext(context_);
                    LinkEnd(widget);
                    recycle.Save(id, type, height, widget);
                }
            }
        }

        recycler_.Insert(recycle);

        Invalidate();
    }

    StList::AdapterContext StList::GetAdapterContext()
    {
        AdapterContext context;
        context.count = adapter_->GetCount();
        int total = 0;
        for (int i = 0; i < context.count; i++)
        {
            int height = adapter_->GetHeight(i);
            if (total + height > scroll_context_.scroll_pos)
            {
                context.head = i;
                context.offset = total - scroll_context_.scroll_pos;
                break;
            }
            total += height;
        }

        int real_height = GetRealHeight();
        total = context.offset;
        for (int i = context.head; i < context.count; i++)
        {
            int height = adapter_->GetHeight(i);
            total += height;

            if (total > real_height)
            {
                context.tail = i + 1;
                break;
            }
        }
        if (context.tail == 0)
        {
            context.tail = context.count;
        }

        return context;
    }


    StList::Recycler::Recycler() {}
    StList::Recycler::~Recycler() {}

    void StList::Recycler::Save(int id, int type, int height, StWidget* widget)
    {
        bool exist = false;
        for (std::vector<Cluster>::iterator iter = clusters_.begin(); iter != clusters_.end(); ++iter)
        {
            if (iter->id == id)
            {
                iter->type = type;
                iter->height = height;
                iter->widget = widget;
                exist = true;
            }
        }

        if (!exist)
        {
            Cluster cluster;
            cluster.id = id;
            cluster.type = type;
            cluster.height = height;
            cluster.widget = widget;
            clusters_.push_back(cluster);
        }
    }

    StList::Recycler::Cluster StList::Recycler::Recycle(int id, bool exact, bool upward)
    {
        Cluster cluster;
        for (std::vector<Cluster>::iterator iter = clusters_.begin(); iter != clusters_.end(); ++iter)
        {
            if (iter->id == id)
            {
                cluster = *iter;
                clusters_.erase(iter);
                return cluster;
            }
        }

        if (!exact && clusters_.size() > 0)
        {
            std::vector<Cluster>::iterator iter;
            if (upward)
            {
                iter = clusters_.begin();
            }
            else
            {
                iter = clusters_.end();
                --iter;
            }
            cluster = *iter;
            clusters_.erase(iter);
            return cluster;
        }

        return cluster;
    }

    void StList::Recycler::Clear()
    {
        for (std::vector<Cluster>::iterator iter = clusters_.begin(); iter != clusters_.end(); ++iter)
        {
            iter->widget->Drop();
        }
        clusters_.clear();
    }

    void StList::Recycler::Insert(StList::Recycler& recycler)
    {
        clusters_.insert(clusters_.begin(), recycler.clusters_.begin(), recycler.clusters_.end());
    }
}