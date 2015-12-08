#include "core/controls/button.h"

namespace core
{
    IMPLY_DYNAMIC_CREATE(StGridButton, "grid-button");



    StGridButton::StGridButton() {}

    StGridButton::~StGridButton() {}

    LRESULT StGridButton::OnMouseHover(WPARAM wparam, LPARAM lparam, bool* handled)
    {
        AddAttribute(FRAME_IMAGE_WIDGET_PROPERTY_INDEX, (int)BS_INDEX_HOVER);
        return StFrameGridImage::OnMouseHover(wparam, lparam, handled);
    }

    LRESULT StGridButton::OnMouseLeave(WPARAM wparam, LPARAM lparam, bool* handled)
    {
        AddAttribute(FRAME_IMAGE_WIDGET_PROPERTY_INDEX, (int)BS_INDEX_NORMAL);
        return StFrameGridImage::OnMouseLeave(wparam, lparam, handled);
    }

    LRESULT StGridButton::OnLButtonDown(WPARAM wparam, LPARAM lparam, bool* handled)
    {
        AddAttribute(FRAME_IMAGE_WIDGET_PROPERTY_INDEX, (int)BS_INDEX_DOWN);
        return StFrameGridImage::OnLButtonDown(wparam, lparam, handled);
    }

    LRESULT StGridButton::OnLButtonUp(WPARAM wparam, LPARAM lparam, bool* handled)
    {
        AddAttribute(FRAME_IMAGE_WIDGET_PROPERTY_INDEX, (int)BS_INDEX_HOVER);
        return StFrameGridImage::OnLButtonUp(wparam, lparam, handled);
    }
};