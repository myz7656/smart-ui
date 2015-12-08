#include "core/layout.h"
#include "core/widget.h"
#include "core/controls/break.h"

namespace core
{
    DWORD LayoutHelper::GetPositionType(StStyle& style)
    {
        if (style.HasAttribute(STYLE_PROPERTY_LEFT) &&
            style.HasAttribute(STYLE_PROPERTY_TOP))
        {
            return POSITION_TYPE_LEFT_TOP;
        }

        if (style.HasAttribute(STYLE_PROPERTY_LEFT) &&
            style.HasAttribute(STYLE_PROPERTY_BOTTOM))
        {
            return POSITION_TYPE_LEFT_BOTTOM;
        }

        if (style.HasAttribute(STYLE_PROPERTY_RIGHT) &&
            style.HasAttribute(STYLE_PROPERTY_TOP))
        {
            return POSITION_TYPE_RIGHT_TOP;
        }

        if (style.HasAttribute(STYLE_PROPERTY_RIGHT) &&
            style.HasAttribute(STYLE_PROPERTY_BOTTOM))
        {
            return POSITION_TYPE_RIGHT_BOTTOM;
        }

        if (style.HasAttribute(STYLE_PROPERTY_LEFT))
        {
            return POSITION_TYPE_LEFT_TOP;
        }

        if (style.HasAttribute(STYLE_PROPERTY_RIGHT))
        {
            return POSITION_TYPE_RIGHT_TOP;
        }

        if (style.HasAttribute(STYLE_PROPERTY_TOP))
        {
            return POSITION_TYPE_LEFT_TOP;
        }

        if (style.HasAttribute(STYLE_PROPERTY_BOTTOM))
        {
            return POSITION_TYPE_LEFT_BOTTOM;
        }

        return POSITION_TYPE_LEFT_TOP;
    }

    int LayoutHelper::GetBaseline(StWidget &widget, bool exclude)
    {
        int baseline = 0;
        StWidget *iterator = widget.PreviousSibling();
        while (iterator)
        {
            if (StBreak::EqualTag(iterator->TagName()))
            {
                break;
            }

            StStyle& style = iterator->LayoutStyle();
            DWORD position = style.Position();
            if (position == POSITION_RELATIVE)
            {
                baseline = max(baseline, iterator->BoundaryRect().bottom);
            }

            iterator = iterator->PreviousSibling();
        }

        if (!exclude)
        {
            baseline = max(baseline, widget.BoundaryRect().bottom);
        }

        iterator = widget.NextSibling();
        while (iterator)
        {
            if (StBreak::EqualTag(iterator->TagName()))
            {
                break;
            }

            StStyle& style = iterator->LayoutStyle();
            DWORD position = style.Position();
            if (position == POSITION_RELATIVE)
            {
                baseline = max(baseline, iterator->BoundaryRect().bottom);
            }

            iterator = iterator->NextSibling();
        }

        return baseline;
    }



    bool StreamLayout::CalculateBox(StStyle& style, RECT parent, BoxParam* box)
    {
        if (!box || parent.left >= parent.right || parent.top >= parent.bottom)
        {
            return false;
        }

        Unit left = style.Left();
        Unit top = style.Top();
        Unit right = style.Right();
        Unit bottom = style.Bottom();

        Unit margin_left = style.MarginLeft();
        Unit margin_top = style.MarginTop();
        Unit margin_right = style.MarginRight();
        Unit margin_bottom = style.MarginBottom();

        int border_width = style.BorderWidth();

        Unit padding_left = style.PaddingLeft();
        Unit padding_top = style.PaddingTop();
        Unit padding_right = style.PaddingRight();
        Unit padding_bottom = style.PaddingBottom();

        Unit width = style.Width();
        Unit height = style.Height();

        int parent_width = parent.right - parent.left;
        int parent_height = parent.bottom - parent.top;

        box->left = left(parent_width);
        box->top = top(parent_height);
        box->right = right(parent_width);
        box->bottom = bottom(parent_height);

        box->margin_left = margin_left(parent_width);
        box->margin_top = margin_top(parent_height);
        box->margin_right = margin_right(parent_width);
        box->margin_bottom = margin_bottom(parent_height);

        box->border = border_width;

        box->padding_left = padding_left(parent_width);
        box->padding_top = padding_top(parent_height);
        box->padding_right = padding_right(parent_width);
        box->padding_bottom = padding_bottom(parent_height);

        box->width = width(parent_width);
        box->height = height(parent_height);

        return true;
    }

    bool StreamLayout::BeforeBlockLayout(StWidget& widget, LayoutParam& parent, BoxParam& box, LayoutRect* rect)
    {
        if (!rect)
        {
            return false;
        }

        StStyle& style = widget.LayoutStyle();
        DWORD position = style.Position();
        if (position == POSITION_ABSOLUTE)
        {
            return BeforeAbsoluteBlockLayout(widget, parent, box, rect);
        }
        else
        {
            return BeforeRelativeBlockLayout(widget, parent, box, rect);
        }

        return false;
    }

    bool StreamLayout::AfterBlockLayout(StWidget& widget, LayoutParam& self)
    {
        StStyle& style = widget.LayoutStyle();

        DWORD position = style.Position();
        if (position == POSITION_ABSOLUTE)
        {
            return AfterAbsoluteBlockLayout(widget, self);
        }
        else
        {
            return AfterRelativeBlockLayout(widget, self);
        }

        return false;
    }

    bool StreamLayout::BeforeAbsoluteBlockLayout(StWidget& widget, LayoutParam& parent, BoxParam& box, LayoutRect* rect)
    {
        if (!rect)
        {
            return false;
        }

        StStyle& style = widget.LayoutStyle();

        int width = box.width;
        int height = box.height;
        DWORD mode = style.BoxMode();
        if (mode == BOX_MODE_CLASSIC)
        {
            width = 2 * box.border + box.padding_left + box.padding_right + box.width;
            height = 2 * box.border + box.padding_top + box.padding_bottom + box.height;
        }

        RECT& rect_parent = parent.content;
        RECT rect_border = {0};

        DWORD position_type = LayoutHelper::GetPositionType(style);
        if (position_type == POSITION_TYPE_LEFT_TOP)
        {
            rect_border.left = rect_parent.left + box.left + box.margin_left;
            rect_border.top = rect_parent.top + box.top + box.margin_top;
            rect_border.right = rect_border.left + width;
            rect_border.bottom = rect_border.top + height;
        }
        else if (position_type == POSITION_TYPE_LEFT_BOTTOM)
        {
            rect_border.left = rect_parent.left + box.left + box.margin_left;
            rect_border.bottom = rect_parent.bottom - box.bottom - box.margin_bottom;
            rect_border.right = rect_border.left + width;
            rect_border.top = rect_border.bottom - height;
        }
        else if (position_type == POSITION_TYPE_RIGHT_TOP)
        {
            rect_border.right = rect_parent.right - box.right - box.margin_right;
            rect_border.top = rect_parent.top + box.top + box.margin_top;
            rect_border.left = rect_border.right - width;
            rect_border.bottom = rect_border.top + height;
        }
        else if(position_type == POSITION_TYPE_RIGHT_BOTTOM)
        {
            rect_border.right = rect_parent.right - box.right - box.margin_right;
            rect_border.bottom = rect_parent.bottom - box.bottom - box.margin_bottom;
            rect_border.left = rect_border.right - width;
            rect_border.top = rect_border.bottom - height;
        }

        rect->border = rect_border;
        rect->boundary = rect->border;

        rect->content.left = rect_border.left + box.padding_left + box.border;
        rect->content.right = rect_border.right - box.padding_right - box.border;
        rect->content.top = rect_border.top + box.padding_top + box.border;
        rect->content.bottom = rect_border.bottom - box.padding_bottom - box.border;

        return true;
    }

    bool StreamLayout::AfterAbsoluteBlockLayout(StWidget& widget, LayoutParam& self)
    {
        if (self.leaf)
        {
            return true;
        }

        DoAlignment(widget, self);
        return true;
    }

    bool StreamLayout::BeforeRelativeBlockLayout(StWidget& widget, LayoutParam& parent, BoxParam& box, LayoutRect* rect)
    {
        StStyle& style = widget.LayoutStyle();
        DWORD mode = style.BoxMode();

        int width = box.width;
        int height = box.height;
        if (mode == BOX_MODE_CLASSIC)
        {
            width = 2 * box.border + box.padding_left + box.padding_right + box.width;
            height = 2 * box.border + box.padding_top + box.padding_bottom + box.height;
        }

        RECT boundary = {0}, border = {0}, content = {0};

        boundary.left = parent.col;
        boundary.top = parent.row;

        border.left = parent.col + box.margin_left;
        border.top = parent.row + box.margin_top;
        border.right = border.left + width;
        border.bottom = border.top + height;

        content.left = border.left + box.padding_left + box.border;
        content.right = border.right - box.padding_right - box.border;
        content.top = border.top + box.padding_top + box.border;
        content.bottom = border.bottom - box.padding_bottom - box.border;

        boundary.right = border.right + box.margin_right;
        boundary.bottom = border.bottom + box.margin_bottom;

        rect->border = border;
        rect->content = content;
        rect->boundary = boundary;

        parent.col = boundary.right;
        parent.baseline = max(parent.baseline, boundary.bottom);

        parent.boundary.left = min(parent.boundary.left, border.left);
        parent.boundary.top = min(parent.boundary.top, border.top);
        parent.boundary.right = max(parent.boundary.right, border.right);
        parent.boundary.bottom = max(parent.boundary.bottom, border.bottom);

        return true;
    }

    bool StreamLayout::AfterRelativeBlockLayout(StWidget& widget, LayoutParam& self)
    {
        if (self.leaf)
        {
            return true;
        }

        DoAlignment(widget, self);

        return true;
    }

    bool StreamLayout::DoAlignment(StWidget& widget, LayoutParam& self)
    {
        StStyle& style = widget.LayoutStyle();

        DWORD horizontal_alignment = style.HorizontalAlignment();
        DWORD vertical_alignment = style.VerticalAlignment();

        int offset_x = 0;
        int offset_y = 0;

        if (self.boundary.left >= self.content.left &&
            self.boundary.right <= self.content.right)
        {
            if ((horizontal_alignment & HORIZONTAL_ALIGNMENT_LEFT) == HORIZONTAL_ALIGNMENT_LEFT)
            {
                offset_x = self.content.left - self.boundary.left;
            }
            else if ((horizontal_alignment & HORIZONTAL_ALIGNMENT_CENTER) == HORIZONTAL_ALIGNMENT_CENTER)
            {
                int width_boundary = self.boundary.right - self.boundary.left;
                int width_content = self.content.right - self.content.left;
                offset_x = (width_content - width_boundary) / 2 - (self.boundary.left - self.content.left);
            }
            else if ((horizontal_alignment & HORIZONTAL_ALIGNMENT_RIGHT) == HORIZONTAL_ALIGNMENT_RIGHT)
            {
                offset_x = self.content.right - self.boundary.right;
            }
        }

        if (self.boundary.top >= self.content.top &&
            self.boundary.bottom <= self.content.bottom)
        {
            if ((vertical_alignment & VERTICAL_ALIGNMENT_TOP) == VERTICAL_ALIGNMENT_TOP)
            {
                offset_y = self.content.top - self.boundary.top;
            }
            else if ((vertical_alignment & VERTICAL_ALIGNMENT_MIDDLE) == VERTICAL_ALIGNMENT_MIDDLE)
            {
                int height_boundary = self.boundary.bottom - self.boundary.top;
                int height_content = self.content.bottom - self.content.top;
                offset_y = (height_content - height_boundary) / 2 - (self.boundary.top - self.content.top);
            }
            else if ((vertical_alignment & VERTICAL_ALIGNMENT_BOTTOM) == VERTICAL_ALIGNMENT_BOTTOM)
            {
                offset_y = self.content.bottom - self.boundary.bottom;
            }
        }

        if (offset_x != 0 || offset_y != 0)
        {
            widget.Move(offset_x, offset_y, true);
        }

        return true;
    }
};