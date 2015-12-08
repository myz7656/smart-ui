#include "core/controls/break.h"

namespace core
{
    IMPLY_DYNAMIC_CREATE(StBreak, "br")



    StBreak::StBreak() {}

    StBreak::~StBreak() {}

    LRESULT StBreak::OnLayout(WPARAM wparam, LPARAM lparam, bool*)
    {
        LayoutParam* layout_param = (LayoutParam*)wparam;
        if (!layout_param)
        {
            return FALSE;
        }

        layout_param->col = layout_param->content.left;
        layout_param->row = layout_param->baseline;
        return TRUE;
    }

    LRESULT StBreak::OnAdjust(WPARAM wparam, LPARAM, bool*)
    {
        AdjustParam *just = (AdjustParam*)wparam;
        if (!just)
        {
            return FALSE;
        }

        just->vertical = true;

        return TRUE;
    }
};