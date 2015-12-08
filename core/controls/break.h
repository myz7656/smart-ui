#ifndef __core_controls_break_h__
#define __core_controls_break_h__

#include "core/def.h"
#include "core/widget.h"
#include <string>

namespace core
{
    class StBreak : public StWidget
    {
    public:
        DECLARE_DYNAMIC_CREATE(StBreak);

        StBreak();
        ~StBreak();

    protected:
        BEGIN_MESSAGE_MAP()
            MESSAGE_HANDLE(ST_LAYOUT, OnLayout)
            MESSAGE_HANDLE(ST_ADJUST, OnAdjust)
            CHAIN_MESSAGE_MAP(StWidget)
        END_MESSAGE_MAP()

        LRESULT OnLayout(WPARAM, LPARAM, bool*);
        LRESULT OnAdjust(WPARAM, LPARAM, bool*);

    private:
        DISABLE_COPY_AND_ASSIGN(StBreak)
    };
};
#endif