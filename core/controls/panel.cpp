#include "core/controls/panel.h"
#include "core/controls/break.h"

namespace core
{
    const char * PANEL_WIDGET_PROPERTY_HSCROLL             = "hscroll";
    const char * PANEL_WIDGET_PROPERTY_VSCROLL             = "vscroll";
    const char * PANEL_WIDGET_PROPERTY_SCROLL_BACKGROUND   = "scroll-background";
    const char * PANEL_WIDGET_PROPERTY_SCROLL_BORDER       = "scroll-border";
    const char * PANEL_WIDGET_PROPERTY_SCROLL_WIDTH        = "scroll-width";


    IMPLY_DYNAMIC_CREATE(StPanel, "panel")



    BEGIN_ATTRIBUTE_MAP(StPanel)
        ATTRIBUTE_ENUM_BEGIN(PANEL_WIDGET_PROPERTY_HSCROLL, StAttributeBool)
            ATTRIBUTE_ENUM_VALUE("enable", true)
            ATTRIBUTE_ENUM_VALUE("disable", false)
        ATTRIBUTE_ENUM_END(ATTRIBUTE_TRAIT_PAINT)

        ATTRIBUTE_ENUM_BEGIN(PANEL_WIDGET_PROPERTY_VSCROLL, StAttributeBool)
            ATTRIBUTE_ENUM_VALUE("enable", true)
            ATTRIBUTE_ENUM_VALUE("disable", false)
        ATTRIBUTE_ENUM_END(ATTRIBUTE_TRAIT_PAINT)

        ATTRIBUTE(PANEL_WIDGET_PROPERTY_SCROLL_BACKGROUND, StAttributeColor, ATTRIBUTE_TRAIT_PAINT)
        ATTRIBUTE(PANEL_WIDGET_PROPERTY_SCROLL_BORDER, StAttributeColor, ATTRIBUTE_TRAIT_PAINT)
        ATTRIBUTE(PANEL_WIDGET_PROPERTY_SCROLL_WIDTH, StAttributePixel, ATTRIBUTE_TRAIT_PAINT)

        CHAIN_ATTRIBUTE_MAP(StContainer)
    END_ATTRIBUTE_MAP()

    StPanel::StPanel()
        : scroll_pos_x_(0)
        , scroll_pos_y_(0)
        , bar_pos_x_(0)
        , bar_pos_y_(0)
        , bar_size_x_(MIN_BAR_SIZE)
        , bar_size_y_(MIN_BAR_SIZE)
        , orientation_(OR_NULL)
        , cursor_pos_x_(0)
        , cursor_pos_y_(0) {}

    StPanel::~StPanel() {}

    StWidget* StPanel::HitTest(int x, int y)
    {
        if (!StWidget::HitTest(x, y))
        {
            return 0;
        }

        if (IsPtInHScroll(x, y) || IsPtInVScroll(x, y))
        {
            return this;
        }

        return StContainer::HitTest(x, y);
    }

    bool StPanel::IsHScrollEnable()
    {
        bool enable;
        GetAttribute(PANEL_WIDGET_PROPERTY_HSCROLL, &enable);
        return enable;
    }

    bool StPanel::IsVScrollEnable()
    {
        bool enable;
        GetAttribute(PANEL_WIDGET_PROPERTY_VSCROLL, &enable);
        return enable;
    }

    core::Color StPanel::ScrollBackground()
    {
        core::Color color;
        GetAttribute(PANEL_WIDGET_PROPERTY_SCROLL_BACKGROUND, &color);
        return color;
    }

    core::Color StPanel::ScrollBorder()
    {
        core::Color color;
        GetAttribute(PANEL_WIDGET_PROPERTY_SCROLL_BORDER, &color);
        return color;
    }

    void StPanel::EnableHScroll(bool enable)
    {
        AddAttribute(PANEL_WIDGET_PROPERTY_HSCROLL, enable);
    }

    void StPanel::EnableVScroll(bool enable)
    {
        AddAttribute(PANEL_WIDGET_PROPERTY_VSCROLL, enable);
    }

    void StPanel::SetScrollBackground(core::Color color)
    {
        AddAttribute(PANEL_WIDGET_PROPERTY_SCROLL_BACKGROUND, color);
    }

    void StPanel::SetScrollBorder(core::Color color)
    {
        AddAttribute(PANEL_WIDGET_PROPERTY_SCROLL_BORDER, color);
    }

    void StPanel::AddWidget(StWidget* widget)
    {
        LinkEnd(widget);
        UpdateLayout(false);
        Invalidate();
    }

    void StPanel::RemoveWidget(StWidget* widget)
    {
        Remove(widget);
        widget->Drop();
        UpdateLayout(false);
        Invalidate();
    }

    LRESULT StPanel::OnLayout(WPARAM wparam, LPARAM lparam, bool* handled)
    {
        LRESULT result = StContainer::OnLayout(wparam, lparam, handled);
        scroll_pos_x_ = 0;
        scroll_pos_y_ = 0;
        UpdateScrollBar();
        return result;
    }

    LRESULT StPanel::OnPaint(WPARAM wparam, LPARAM lparam, bool* handled)
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

    LRESULT StPanel::OnMouseHover(WPARAM wparam, LPARAM lparam, bool* handled)
    {
        if (IsCapture())
        {
            int x = ((int)(short)LOWORD(lparam));
            int y = ((int)(short)HIWORD(lparam));

            x = max(x, layout_rect_.content.left);
            x = min(x, layout_rect_.content.right);

            y = max(y, layout_rect_.content.top);
            y = min(y, layout_rect_.content.bottom);

            int width = GetRealWidth();
            int height = GetRealHeight();

            if (orientation_ == OR_HORIZONTAL)
            {
                int offset = x - cursor_pos_x_;
                cursor_pos_x_ = x;

                bar_pos_x_ += offset;

                bar_pos_x_ = max(0, bar_pos_x_);
                bar_pos_x_ = min(bar_pos_x_, width - bar_size_x_);

                ScrollView();
            }
            else if (orientation_ == OR_VERTICAL)
            {
                int offset = y - cursor_pos_y_;
                cursor_pos_y_ = y;

                int last_bar_pos_y = bar_pos_y_;

                bar_pos_y_ += offset;

                bar_pos_y_ = max(0, bar_pos_y_);
                bar_pos_y_ = min(bar_pos_y_, height - bar_size_y_);

                offset = bar_pos_y_ - last_bar_pos_y;

                ScrollView();
            }
            Invalidate();
            return TRUE;
        }
        else
        {
            return StContainer::OnMouseHover(wparam, lparam, handled);
        }
    }

    LRESULT StPanel::OnMouseLeave(WPARAM wparam, LPARAM lparam, bool* handled)
    {
        return StContainer::OnMouseLeave(wparam, lparam, handled);
    }

    LRESULT StPanel::OnLButtonDown(WPARAM wparam, LPARAM lparam, bool* handled)
    {
        int x = ((int)(short)LOWORD(lparam));
        int y = ((int)(short)HIWORD(lparam));

        bool capture = false;
        if (IsPtInHScroll(x, y))
        {
            orientation_ = OR_HORIZONTAL;
            capture = true;
        }
        else if (IsPtInVScroll(x, y))
        {
            orientation_ = OR_VERTICAL;
            capture = true;
        }

        if (capture)
        {
            SetCapture(true);
            cursor_pos_x_ = x;
            cursor_pos_y_ = y;
        }

        return StContainer::OnLButtonDown(wparam, lparam, handled);
    }

    LRESULT StPanel::OnLButtonUp(WPARAM wparam, LPARAM lparam, bool* handled)
    {
        if (IsCapture())
        {
            SetCapture(false);
            orientation_ = OR_NULL;
            cursor_pos_x_ = 0;
            cursor_pos_y_ = 0;
        }

        return StContainer::OnLButtonUp(wparam, lparam, handled);
    }

    LRESULT StPanel::OnMouseWheel(WPARAM wparam, LPARAM, bool*)
    {
        int offset = scroll_pos_y_;
        int delta = (int)(short)HIWORD(wparam);
        scroll_pos_y_ -= delta;

        int total_height = GetTotalHeight();
        int height = GetRealHeight();

        scroll_pos_y_ = min(scroll_pos_y_, total_height - height);
        scroll_pos_y_ = max(0, scroll_pos_y_);

        offset -= scroll_pos_y_;

        Move(0, offset, true);

        UpdateScrollBar();

        Invalidate();
        return TRUE;
    }

    int StPanel::GetTotalHeight()
    {
        int top = 0;
        int bottom = 0;
        StWidget* widget = FirstChild();
        while (widget)
        {
            if (StBreak::EqualTag(widget->TagName()))
            {
                widget = widget->NextSibling();
                continue;
            }
            RECT boundary = widget->BoundaryRect();
            
            if (top == 0)
            {
                top = boundary.top;
            }
            if (bottom == 0)
            {
                bottom = boundary.bottom;
            }
            top = min(top, boundary.top);
            bottom = max(bottom, boundary.bottom);

            widget = widget->NextSibling();
        }
        return bottom - top;
    }

    int StPanel::GetTotalWidth()
    {
        int left = 0;
        int right = 0;
        StWidget* widget = FirstChild();
        while (widget)
        {
            if (StBreak::EqualTag(widget->TagName()))
            {
                widget = widget->NextSibling();
                continue;
            }

            RECT boundary = widget->BoundaryRect();
            if (left == 0)
            {
                left = boundary.left;
            }
            if (right == 0)
            {
                right = boundary.right;
            }

            left = min(left, boundary.left);
            right = max(right, boundary.right);

            widget = widget->NextSibling();
        }
        return right - left;
    }

    int StPanel::GetRealHeight()
    {
        return layout_rect_.content.bottom - layout_rect_.content.top;
    }

    int StPanel::GetRealWidth()
    {
        return layout_rect_.content.right - layout_rect_.content.left;
    }

    bool StPanel::IsPtInHScroll(int x, int y)
    {
        if (!IsHScrollEnable())
        {
            return false;
        }

        if (bar_size_x_ == 0)
        {
            return false;
        }

        RECT rect = GetHScrollRect();

        POINT pt = {x, y};
        if (::PtInRect(&rect, pt))
        {
            return true;
        }
        return false;
    }

    bool StPanel::IsPtInVScroll(int x, int y)
    {
        if (!IsVScrollEnable())
        {
            return false;
        }

        if (bar_size_y_ == 0)
        {
            return false;
        }

        RECT rect = GetVScrollRect();

        POINT pt = {x, y};
        if (::PtInRect(&rect, pt))
        {
            return true;
        }
        return false;
    }

    void StPanel::UpdateScrollBar()
    {
        bar_pos_x_ = 0;
        bar_size_x_ = 0;
        bar_pos_y_ = 0;
        bar_size_y_ = 0;

        int height = GetRealHeight();
        int width = GetRealWidth();

        int total_height = GetTotalHeight();
        int total_width = GetTotalWidth();

        if (total_width > width)
        {
            double size_x = (double)width / (double)total_width * (double)width;
            bar_size_x_ = (int)size_x;
            bar_size_x_ = max(bar_size_x_, MIN_BAR_SIZE);

            int scroll_range = total_width - width;
            int bar_range = width - bar_size_x_;

            double bar_pos = (double)scroll_pos_x_ / (double)scroll_range * (double)bar_range;
            bar_pos_x_ = (int)bar_pos;
        }

        if (total_height > height)
        {
            double size_y = (double)height / (double)total_height * (double)height;
            bar_size_y_ = (int)size_y;
            bar_size_y_ = max(bar_size_y_, MIN_BAR_SIZE);

            int scroll_range = total_height - height;
            int bar_range = height - bar_size_y_;

            double bar_pos = (double)scroll_pos_y_ / (double)scroll_range * (double)bar_range;
            bar_pos_y_ = (int)bar_pos;
        }
    }

    void StPanel::ScrollView()
    {
        int width = GetRealWidth();
        int height = GetRealHeight();

        int total_height = GetTotalHeight();
        int total_width = GetTotalWidth();

        double scroll_x = (double)bar_pos_x_ / (double)width * (double)total_width;
        double scroll_y = (double)bar_pos_y_ / (double)height * (double)total_height;

        int last_view_pos_x = scroll_pos_x_;
        int last_view_pos_y = scroll_pos_y_;

        scroll_pos_x_ = (int)scroll_x;
        scroll_pos_y_ = (int)scroll_y;

        scroll_pos_x_ = max(0, scroll_pos_x_);
        scroll_pos_x_ = min(scroll_pos_x_, total_width - width);

        scroll_pos_y_ = max(0, scroll_pos_y_);
        scroll_pos_y_ = min(scroll_pos_y_, total_height - height);

        int move_x = last_view_pos_x - scroll_pos_x_;
        int move_y = last_view_pos_y - scroll_pos_y_;
        Move(move_x, move_y, true);
    }

    void StPanel::DrawScroll(StPainter* painter)
    {
        core::Color background = ScrollBackground();
        core::Color border = ScrollBorder();

        if (IsVScrollEnable())
        {
            RECT rect = GetVScrollRect();

            painter->DrawBackground(rect, background, 255);
            painter->DrawBorder(rect, BORDER_STYLE_SOLID, 1, border, 255);
        }

        if (IsHScrollEnable())
        {
            RECT rect = GetHScrollRect();

            painter->DrawBackground(rect, background, 255);
            painter->DrawBorder(rect, BORDER_STYLE_SOLID, 1, border, 255);
        }
    }

    RECT StPanel::GetVScrollRect()
    {
        RECT rect;
        rect.top = layout_rect_.content.top + bar_pos_y_;
        rect.bottom = rect.top + bar_size_y_;
        rect.right = layout_rect_.content.right;

        int nWidth = DEFAULT_BAR_WIDTH;
        GetAttribute(PANEL_WIDGET_PROPERTY_SCROLL_WIDTH, &nWidth, DEFAULT_BAR_WIDTH);
        rect.left = rect.right - nWidth;
        return rect;
    }

    RECT StPanel::GetHScrollRect()
    {
        RECT rect;
        rect.left = layout_rect_.content.left + bar_pos_x_;
        rect.right = rect.left + bar_size_x_;
        rect.bottom = layout_rect_.content.bottom;

        int nWidth = DEFAULT_BAR_WIDTH;
        GetAttribute(PANEL_WIDGET_PROPERTY_SCROLL_WIDTH, &nWidth, DEFAULT_BAR_WIDTH);
        rect.top = rect.bottom - nWidth;
        return rect;
    }
}