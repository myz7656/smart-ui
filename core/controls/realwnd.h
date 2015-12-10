#ifndef __core_controls_realwnd_h__
#define __core_controls_realwnd_h__

#include "core/def.h"
#include "core/widget.h"

#include <string>

namespace core
{
    class StRealWnd : public StWidget
    {
    public:
        DECLARE_DYNAMIC_CREATE(StRealWnd);

        StRealWnd();
        ~StRealWnd();

        void SetHWnd(HWND hWnd);
        HWND GetHWnd();

        virtual void Move(int x, int y, bool onlyChild);

    protected:
        BEGIN_MESSAGE_MAP()
            MESSAGE_HANDLE(ST_LAYOUT, OnLayout)
            MESSAGE_HANDLE(ST_PAINT, OnPaint)
            MESSAGE_HANDLE(ST_SHOW, OnShow)
            CHAIN_MESSAGE_MAP(StWidget)
        END_MESSAGE_MAP()

        LRESULT OnLayout(WPARAM, LPARAM, bool*);
        LRESULT OnPaint(WPARAM, LPARAM, bool*);
        LRESULT OnShow(WPARAM, LPARAM, bool*);

    protected:
        HWND hwnd_;

    private:
        DISABLE_COPY_AND_ASSIGN(StRealWnd)
    };
};
#endif