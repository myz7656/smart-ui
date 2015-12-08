#ifndef __core_controls_hr_h__
#define __core_controls_hr_h__

#include "core/def.h"
#include "core/widget.h"

#include <string>

namespace core
{
    class StHr : public StWidget
    {
    public:
        DECLARE_DYNAMIC_CREATE(StHr);

        StHr();
        ~StHr();

        virtual StObject* Clone();

    protected:
        BEGIN_MESSAGE_MAP()
            MESSAGE_HANDLE(ST_CALCULATE, OnCalculate)
            MESSAGE_HANDLE(ST_PAINT, OnPaint)
            CHAIN_MESSAGE_MAP(StWidget)
        END_MESSAGE_MAP()

        LRESULT OnCalculate(WPARAM, LPARAM, bool*);
        LRESULT OnPaint(WPARAM, LPARAM, bool*);

        void DrawLine(StPainter* painter, RECT* clip);

    private:
        DISABLE_COPY_AND_ASSIGN(StHr)
    };
};
#endif