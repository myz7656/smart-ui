#include "core/widget.h"

namespace core
{
    const char* WIDGET_PROPERTY_ID              = "id";
    const char* WIDGET_PROPERTY_VISIBLE         = "visible";
    const char* WIDGET_PROPERTY_TITLE           = "title";
    const char* WIDGET_PROPERTY_TABSTOP         = "tabstop";
    const char* WIDGET_PROPERTY_ZINDEX          = "z-index";
    const char* WIDGET_PROPERTY_ALPHA           = "alpha";
    const char* WIDGET_PROPERTY_DRAG            = "drag";
    const char* WIDGET_PROPERTY_FILTER_EVENT    = "filter-event";

    BEGIN_ATTRIBUTE_MAP(StWidget)
        ATTRIBUTE(WIDGET_PROPERTY_ID, StAttributeString, ATTRIBUTE_TRAIT_NULL)

        ATTRIBUTE_ENUM_BEGIN(WIDGET_PROPERTY_VISIBLE, StAttributeBool)
            ATTRIBUTE_ENUM_VALUE("visible", true)
            ATTRIBUTE_ENUM_VALUE("hidden", false)
        ATTRIBUTE_ENUM_END(ATTRIBUTE_TRAIT_PAINT)

        ATTRIBUTE(WIDGET_PROPERTY_TITLE, StAttributeWString, ATTRIBUTE_TRAIT_NULL)

        ATTRIBUTE_ENUM_BEGIN(WIDGET_PROPERTY_TABSTOP, StAttributeBool)
            ATTRIBUTE_ENUM_VALUE("enable", true)
            ATTRIBUTE_ENUM_VALUE("disable", false)
        ATTRIBUTE_ENUM_END(ATTRIBUTE_TRAIT_NULL)

        ATTRIBUTE_ENUM_BEGIN(WIDGET_PROPERTY_DRAG, StAttributeBool)
            ATTRIBUTE_ENUM_VALUE("enable", true)
            ATTRIBUTE_ENUM_VALUE("disable", false)
        ATTRIBUTE_ENUM_END(ATTRIBUTE_TRAIT_NULL)

        ATTRIBUTE(WIDGET_PROPERTY_ZINDEX, StAttributeInteger, ATTRIBUTE_TRAIT_PAINT)

        ATTRIBUTE(WIDGET_PROPERTY_ALPHA, StAttributeInteger, ATTRIBUTE_TRAIT_PAINT)

        ATTRIBUTE_ENUM_BEGIN(WIDGET_PROPERTY_FILTER_EVENT, StAttributeBool)
            ATTRIBUTE_ENUM_VALUE("filter", true)
            ATTRIBUTE_ENUM_VALUE("no-filter", false)
        ATTRIBUTE_ENUM_END(ATTRIBUTE_TRAIT_NULL)

    END_ATTRIBUTE_MAP()

    StWidget::StWidget()
        : context_(0)
        , extra_(0)
        , layoutor_(0)
        , action_state_(ACTION_STATE_NORMAL)
        , focus_(false)
        , capture_(false)
        , parent_(0)
        , prev_(0)
        , next_(0) {}

    StWidget::~StWidget() {}

    StObject* StWidget::Clone()
    {
        StWidget* widget = new StWidget;
        if (!widget)
        {
            return 0;
        }

        CopyTo(widget);

        return widget;
    }

    void StWidget::Drop()
    {
        if (extra_)
        {
            extra_->Drop();
        }
        if (context_)
        {
            context_->RemovedWidget(this);
        }
        delete this;
    }

    bool StWidget::IsContainer()
    {
        return false;
    }

    StWidget* StWidget::HitTest(int x, int y)
    {
        if (IsFilterEvent() || !IsVisible())
        {
            return 0;
        }

        POINT pt = {x, y};
        if (::PtInRect(&layout_rect_.border, pt))
        {
            return this;
        }
        return 0;
    }

    void StWidget::Move(int x, int y, bool onlyChild)
    {
        OffsetRect(&layout_rect_.content, x, y);
        OffsetRect(&layout_rect_.border, x, y);
        OffsetRect(&layout_rect_.boundary, x, y);

        layout_param_.Offset(x, y);
    }


    Context* StWidget::GetContext()
    {
        return context_;
    }

    void StWidget::SetContext(Context* context)
    {
        context_ = context;
    }

    StWidget::MetaData* StWidget::ExtraData()
    {
        return extra_;
    }

    void StWidget::SetExtraData(MetaData* data)
    {
        extra_ = data;
    }

    StStyle& StWidget::GetStyle(DWORD state)
    {
        switch (state)
        {
        case core::ACTION_STATE_NORMAL:
            return style_normal_;

        case core::ACTION_STATE_HOVER:
            return style_hover_;

        case core::ACTION_STATE_DOWN:
            return style_down_;

        default:
            return style_normal_;
        }
    }

    StStyle& StWidget::LayoutStyle()
    {
        return GetStyle(ACTION_STATE_NORMAL);
    }

    RECT StWidget::ContentRect()
    {
        return layout_rect_.content;
    }

    RECT StWidget::BorderRect()
    {
        return layout_rect_.border;
    }

    RECT StWidget::BoundaryRect()
    {
        return layout_rect_.boundary;
    }

    ILayout* StWidget::Layoutor()
    {
        return layoutor_;
    }

    void StWidget::SetLayoutor(ILayout* layout)
    {
        layoutor_ = layout;
    }

    DWORD StWidget::ActionState()
    {
        return action_state_;
    }

    void StWidget::SetActionState(DWORD state)
    {
        if (state != action_state_)
        {
            action_state_ = state;
            Invalidate();
        }
    }

    bool StWidget::IsFocused()
    {
        return focus_;
    }

    void StWidget::SetFocus(bool focus)
    {
        focus_ = focus;
    }

    bool StWidget::IsCapture()
    {
        return capture_;
    }

    void StWidget::SetCapture(bool capture)
    {
        capture_ = capture;
    }

    const std::string& StWidget::TagName()
    {
        return tag_name_;
    }

    void StWidget::SetTagName(const std::string &name)
    {
        tag_name_ = name;
    }

    StWidget* StWidget::Parent()
    {
        return parent_;
    }

    StWidget* StWidget::PreviousSibling()
    {
        return prev_;
    }

    StWidget* StWidget::NextSibling()
    {
        return next_;
    }

    void StWidget::AddMessageObserver(IMessageObserver* observer)
    {
        if (observer)
        {
            observers_.push_back(observer);
        }
    }

    void StWidget::RemoveMessageObserver(IMessageObserver* observer)
    {
        if (!observer)
        {
            return;
        }
        MessageObservers::iterator iter = observers_.begin();
        while (iter != observers_.end())
        {
            if (observer == *iter)
            {
                observers_.erase(iter);
                break;
            }
            ++iter;
        }
    }

    void StWidget::NotifyObservers(UINT umsg, WPARAM wparam, LPARAM lparam, bool before)
    {
        MessageObservers::iterator iter = observers_.begin();
        while (iter != observers_.end())
        {
            (*iter)->OnMessageObserver(this, umsg, wparam, lparam, before);
            ++iter;
        }
    }


    KeyWidget StWidget::Key()
    {
        KeyWidget key;
        std::string id = Id();
        if (!id.empty())
        {
            key = StringToKeyWidget(id);
        }
        return key;
    }

    std::string StWidget::Id()
    {
        std::string id;
        GetAttribute(WIDGET_PROPERTY_ID, &id);
        return id;
    }

    void StWidget::SetId(const std::string& id)
    {
        AddAttribute(WIDGET_PROPERTY_ID, id);
    }

    bool StWidget::IsVisible()
    {
        bool visible = true;
        GetAttribute(WIDGET_PROPERTY_VISIBLE, &visible, true);
        return visible;
    }

    void StWidget::SetVisible(bool visible)
    {
        AddAttribute(WIDGET_PROPERTY_VISIBLE, visible);
    }

    std::wstring StWidget::Title()
    {
        std::wstring title;
        GetAttribute(WIDGET_PROPERTY_TITLE, &title);
        return title;
    }

    void StWidget::SetTitle(const std::wstring& title)
    {
        AddAttribute(WIDGET_PROPERTY_TITLE, title);
    }

    bool StWidget::IsTabstop()
    {
        bool tabstop = false;
        GetAttribute(WIDGET_PROPERTY_TABSTOP, &tabstop, false);
        return tabstop;
    }

    void StWidget::SetTabStop(bool enable)
    {
        AddAttribute(WIDGET_PROPERTY_TABSTOP, enable);
    }

    bool StWidget::IsDraggable()
    {
        bool draggable = false;
        GetAttribute(WIDGET_PROPERTY_DRAG, &draggable, false);
        return draggable;
    }

    void StWidget::SetDraggable(bool draggable)
    {
        AddAttribute(WIDGET_PROPERTY_DRAG, draggable);
    }

    int StWidget::ZIndex()
    {
        int z_index = 0;
        GetAttribute(WIDGET_PROPERTY_ZINDEX, &z_index, 0);
        return z_index;
    }

    void StWidget::SetZIndex(int z_index)
    {
        AddAttribute(WIDGET_PROPERTY_ZINDEX, z_index);
    }

    int StWidget::Alpha()
    {
        int alpha = 255;
        GetAttribute(WIDGET_PROPERTY_ALPHA, &alpha, 255);
        return alpha;
    }

    void StWidget::SetAlpha(int alpha)
    {
        AddAttribute(WIDGET_PROPERTY_ALPHA, alpha);
    }

    bool StWidget::IsFilterEvent()
    {
        bool filter = false;
        GetAttribute(WIDGET_PROPERTY_FILTER_EVENT, &filter, false);
        return filter;
    }

    void StWidget::SetFilterEvent(bool filter)
    {
        AddAttribute(WIDGET_PROPERTY_FILTER_EVENT, filter);
    }


    LRESULT StWidget::Dispatch(UINT umsg, WPARAM wparam, LPARAM lparam)
    {
        NotifyObservers(umsg, wparam, lparam, true);

        LRESULT result = 0;
        ProcessMessage(umsg, wparam, lparam, &result);

        NotifyObservers(umsg, wparam, lparam, false);
        return result;
    }

    void StWidget::ReLayoutSelf(bool repaint)
    {
        LayoutParam layout_param = layout_param_;
        Dispatch(ST_LAYOUT, (WPARAM)&layout_param, 0);

        if (repaint)
        {
            Invalidate();
        }
    }

    void StWidget::UpdateLayout(bool repaint)
    {
        RECT rect_update = layout_rect_.border;

        RECT before_boundary = layout_rect_.boundary;
        int oldbaseline = LayoutHelper::GetBaseline(*this, false);

        ReLayoutSelf(false);

        ::UnionRect(&rect_update, &rect_update, &layout_rect_.border);

        StStyle& style = LayoutStyle();
        DWORD postion = style.Position();

        do 
        {
            if (postion == POSITION_ABSOLUTE)
            {
                break;
            }

            RECT after_boundary = layout_rect_.boundary;
            int newbaseline = LayoutHelper::GetBaseline(*this, false);

            int before_width = before_boundary.right - before_boundary.left;
            int after_width = after_boundary.right - after_boundary.left;
            int x = after_width - before_width + after_boundary.left - before_boundary.left;
            int y = newbaseline - oldbaseline;
            if (x == 0 && y == 0)
            {
                break;
            }

            AdjustParam just;
            just.x = x;
            just.y = y;
            just.vertical = false;

            for (StWidget* widget = next_; widget != 0; widget = widget->next_)
            {
                RECT rect_before = widget->BorderRect();
                ::UnionRect(&rect_update, &rect_update, &rect_before);

                if (!widget->Dispatch(ST_ADJUST, (WPARAM)&just, 0))
                {
                    break;
                }

                RECT rect_after = widget->BorderRect();
                ::UnionRect(&rect_update, &rect_update, &rect_after);
            }

        } while (0);

        if (repaint)
        {
            InvalidateRect(rect_update);
        }
    }

    void StWidget::Invalidate()
    {
        InvalidateRect(layout_rect_.boundary);
    }

    void StWidget::InvalidateRect(RECT &rect)
    {
        if (rect.right > rect.left && rect.bottom > rect.top)
        {
            HWND hwnd = context_->GetHWND();
            ::InvalidateRect(hwnd, &rect, FALSE);
        }
    }

    void StWidget::Inherit(StWidget& widget)
    {
        style_normal_.Inherit(widget.style_normal_);
        style_hover_.Inherit(widget.style_hover_);
        style_down_.Inherit(widget.style_down_);

        if (widget.HasAttribute(WIDGET_PROPERTY_ALPHA))
        {
            int alpha = widget.Alpha();
            AddAttribute(WIDGET_PROPERTY_ALPHA, alpha);
        }
    }

    LayoutParam StWidget::GetLayoutParam()
    {
        return layout_param_;
    }

    void StWidget::MoveTo(int x, int y)
    {
        RECT boundary = BoundaryRect();
        int left = boundary.left;
        int top = boundary.top;

        int offset_x = x - left;
        int offset_y = y - top;

        Move(offset_x, offset_y, false);
    }

    void StWidget::CopyTo(StWidget* target)
    {
        StAttributeSet::CopyTo(target);
        target->style_normal_ = style_normal_;
        target->style_hover_ = style_hover_;
        target->style_down_ = style_down_;

        target->tag_name_ =  tag_name_;
    }

    int StWidget::BeginPaint(StPainter *painter, RECT *clip)
    {
        if (!painter || !clip)
        {
            return 0;
        }

        RECT intersect = {0};
        ::IntersectRect(&intersect, clip, &layout_rect_.border);
        bool valid = (intersect.right > intersect.left) && (intersect.bottom > intersect.top);
        if (!valid)
        {
            return 0;
        }

        return painter->SetClipRect(layout_rect_.border);
    }

    void StWidget::DrawSelf(StPainter *painter)
    {
        DrawBackground(painter);
        DrawBorder(painter);
    }

    void StWidget::DrawBackground(StPainter *painter)
    {
        StStyle& style = GetStyle(ActionState());
        int alpha = Alpha();

        RECT background = layout_rect_.border;
        std::wstring background_image = style.BackgroundImage();
        if (style.HasAttribute(STYLE_PROPERTY_BACKGROUND_COLOR))
        {
            core::Color background_color = style.BackgroundColor();

            if (style.HasAttribute(STYLE_PROPERTY_BORDER_RADIUS_WIDTH))
            {
                int nRadiusWidth = style.BorderRadiusWidth();
                int nRadiusHeight = style.BorderRadiusHeight();
                painter->DrawRoundRect(background, nRadiusWidth, nRadiusHeight, background_color, alpha);
            }
            else
            {
                painter->DrawBackground(background, background_color, alpha);
            }
        }
        if (!background_image.empty())
        {
            painter->DrawImage(background_image, background, alpha);
        }
    }

    void StWidget::DrawBorder(StPainter* painter)
    {
        StStyle& style = GetStyle(ActionState());
        int border_width = style.BorderWidth();
        if (border_width > 0)
        {
            DWORD border_style = style.BorderStyle();
            core::Color border_color = style.BorderColor();
            int alpha = Alpha();

            if (style.HasAttribute(STYLE_PROPERTY_BORDER_RADIUS_WIDTH))
            {
                int nRadiusWidth = style.BorderRadiusWidth();
                int nRadiusHeight = style.BorderRadiusHeight();
                painter->DrawRoundBorder(
                    layout_rect_.border,
                    nRadiusWidth, 
                    nRadiusHeight, 
                    border_style, 
                    border_width, 
                    border_color, 
                    alpha);
            }
            else
            {
                painter->DrawBorder(
                    layout_rect_.border,
                    border_style,
                    border_width,
                    border_color,
                    alpha);
            }
        }
    }

    void StWidget::EndPaint(StPainter *painter, int clip)
    {
        if (painter)
        {
            painter->ClearClipRect(clip);
        }
    }

    LRESULT StWidget::OnCalculate(WPARAM wparam, LPARAM lparam, bool*)
    {
        LayoutParam* layout_param = (LayoutParam*)wparam;
        BoxParam* box_param = (BoxParam*)lparam;
        if (!layout_param || !box_param)
        {
            return FALSE;
        }
        StStyle& style = LayoutStyle();
        ILayout* layoutor = Layoutor();
        if (layoutor)
        {
            layoutor->CalculateBox(style, layout_param->content, box_param);
        }
        else
        {
            StreamLayout layoutor;
            layoutor.CalculateBox(style, layout_param->content, box_param);
        }
        return TRUE;
    }

    LRESULT StWidget::OnLayout(WPARAM wparam, LPARAM lparam, bool*)
    {
        LayoutParam* layout_param = (LayoutParam*)wparam;
        if (!layout_param)
        {
            return FALSE;
        }
        layout_param_ = *layout_param;

        if (layout_param_.content.right - layout_param_.content.left == 0 || 
            layout_param_.content.bottom - layout_param_.content.top == 0)
        {
            return FALSE;
        }

        BoxParam box_param;
        LRESULT result = Dispatch(ST_CALCULATE, (WPARAM)layout_param, (LPARAM)&box_param);
        if (!result)
        {
            return FALSE;
        }

        layout_rect_.Reset();
        ILayout* layoutor = Layoutor();
        if (layoutor)
        {
            layoutor->BeforeBlockLayout(*this, *layout_param, box_param, &layout_rect_);
        }
        else
        {
            StreamLayout layoutor;
            layoutor.BeforeBlockLayout(*this, *layout_param, box_param, &layout_rect_);
        }
        return TRUE;
    }

    LRESULT StWidget::OnAdjust(WPARAM wparam, LPARAM, bool*)
    {
        AdjustParam* just = (AdjustParam*)wparam;
        if (!just)
        {
            return FALSE;
        }

        int x = 0;
        int y = 0;

        if (just->vertical)
        {
            y = just->y;
        }
        else
        {
            x = just->x;
        }

        Move(x, y, false);

        return true;
    }

    LRESULT StWidget::OnPaint(WPARAM wparam, LPARAM lparam, bool*)
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

        EndPaint(painter, nclip);

        return TRUE;
    }

    LRESULT StWidget::OnMouseHover(WPARAM, LPARAM, bool*)
    {
        SetActionState(ACTION_STATE_HOVER);
        return TRUE;
    }

    LRESULT StWidget::OnMouseLeave(WPARAM, LPARAM, bool*)
    {
        SetActionState(ACTION_STATE_NORMAL);
        return TRUE;
    }

    LRESULT StWidget::OnLButtonDown(WPARAM, LPARAM, bool*)
    {
        SetActionState(ACTION_STATE_DOWN);
        return TRUE;
    }

    LRESULT StWidget::OnLButtonUp(WPARAM, LPARAM, bool*)
    {
        SetActionState(ACTION_STATE_HOVER);
        return TRUE;
    }

    LRESULT StWidget::OnMouseWheel(WPARAM, LPARAM, bool*)
    {
        return TRUE;
    }

    LRESULT StWidget::OnRButtonDown(WPARAM, LPARAM, bool*)
    {
        return TRUE;
    }

    LRESULT StWidget::OnRButtonUp(WPARAM, LPARAM, bool*)
    {
        return TRUE;
    }

    LRESULT StWidget::OnKeyDown(WPARAM, LPARAM, bool*)
    {
        return TRUE;
    }

    LRESULT StWidget::OnKeyUp(WPARAM, LPARAM, bool*)
    {
        return TRUE;
    }

    LRESULT StWidget::OnChar(WPARAM, LPARAM, bool*)
    {
        return TRUE;
    }
}