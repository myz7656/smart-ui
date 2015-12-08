#ifndef __core_panel_h__
#define __core_panel_h__

#include "core/def.h"
#include "core/container.h"

namespace core
{
    extern const char * PANEL_WIDGET_PROPERTY_HSCROLL;
    extern const char * PANEL_WIDGET_PROPERTY_VSCROLL;
    extern const char * PANEL_WIDGET_PROPERTY_SCROLL_BACKGROUND;
    extern const char * PANEL_WIDGET_PROPERTY_SCROLL_BORDER;
    extern const char * PANEL_WIDGET_PROPERTY_SCROLL_WIDTH;


    class StPanel
        : public StContainer
    {
    public:
        DECLARE_DYNAMIC_CREATE(StPanel);

        const static int MIN_BAR_SIZE = 20;
        const static int DEFAULT_BAR_WIDTH = 10;

        enum Orientation
        {
            OR_NULL,
            OR_HORIZONTAL,
            OR_VERTICAL
        };

        StPanel();
        ~StPanel();

        virtual StWidget* HitTest(int x, int y);

        bool IsHScrollEnable();
        bool IsVScrollEnable();
        core::Color ScrollBackground();
        core::Color ScrollBorder();

        void EnableHScroll(bool enable);
        void EnableVScroll(bool enable);
        void SetScrollBackground(core::Color color);
        void SetScrollBorder(core::Color color);

        void AddWidget(StWidget* widget);
        void RemoveWidget(StWidget* widget);

    protected:
        DECLARE_ATTRIBUTE_MAP()

        BEGIN_MESSAGE_MAP()
            MESSAGE_HANDLE(ST_LAYOUT, OnLayout)
            MESSAGE_HANDLE(ST_PAINT, OnPaint)
            MESSAGE_HANDLE(ST_MOUSEHOVER, OnMouseHover)
            MESSAGE_HANDLE(ST_MOUSELEAVE, OnMouseLeave)
            MESSAGE_HANDLE(ST_LBUTTONDOWN, OnLButtonDown)
            MESSAGE_HANDLE(ST_LBUTTONUP, OnLButtonUp)
            MESSAGE_HANDLE(ST_MOUSEWHEEL, OnMouseWheel)
            CHAIN_MESSAGE_MAP(StContainer)
        END_MESSAGE_MAP()

        LRESULT OnLayout(WPARAM, LPARAM, bool*);
        LRESULT OnPaint(WPARAM, LPARAM, bool*);
        LRESULT OnMouseHover(WPARAM, LPARAM, bool*);
        LRESULT OnMouseLeave(WPARAM, LPARAM, bool*);
        LRESULT OnLButtonDown(WPARAM, LPARAM, bool*);
        LRESULT OnLButtonUp(WPARAM, LPARAM, bool*);
        LRESULT OnMouseWheel(WPARAM, LPARAM, bool*);

        int GetTotalWidth();
        int GetTotalHeight();
        int GetRealWidth();
        int GetRealHeight();

        bool IsPtInHScroll(int x, int y);
        bool IsPtInVScroll(int x, int y);

        void UpdateScrollBar();
        void ScrollView();

        void DrawScroll(StPainter* painter);
        RECT GetVScrollRect();
        RECT GetHScrollRect();

    protected:
        int scroll_pos_x_;
        int scroll_pos_y_;

        int bar_pos_x_;
        int bar_pos_y_;
        int bar_size_x_;
        int bar_size_y_;

        Orientation orientation_;
        int cursor_pos_x_;
        int cursor_pos_y_;

    private:
        DISABLE_COPY_AND_ASSIGN(StPanel)
    };
}

#endif