#ifndef __core_controls_button_h__
#define __core_controls_button_h__

#include "core/controls/img.h"

namespace core
{
    class StGridButton : public StFrameGridImage
    {
    public:
        DECLARE_DYNAMIC_CREATE(StGridButton)

        enum {BS_INDEX_NORMAL = 0, BS_INDEX_HOVER = 1, BS_INDEX_DOWN = 2};

        StGridButton();
        ~StGridButton();

    protected:
        BEGIN_MESSAGE_MAP()
            MESSAGE_HANDLE(ST_MOUSEHOVER,      OnMouseHover )
            MESSAGE_HANDLE(ST_MOUSELEAVE,      OnMouseLeave )
            MESSAGE_HANDLE(ST_LBUTTONDOWN,     OnLButtonDown)
            MESSAGE_HANDLE(ST_LBUTTONUP,       OnLButtonUp  )
            CHAIN_MESSAGE_MAP(StFrameGridImage)
        END_MESSAGE_MAP()

        LRESULT OnMouseHover (WPARAM, LPARAM, bool*);
        LRESULT OnMouseLeave (WPARAM, LPARAM, bool*);
        LRESULT OnLButtonDown(WPARAM, LPARAM, bool*);
        LRESULT OnLButtonUp  (WPARAM, LPARAM, bool*);

    private:
        DISABLE_COPY_AND_ASSIGN(StGridButton)
    };
};
#endif