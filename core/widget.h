#ifndef __core_widget_h__
#define __core_widget_h__

#include <vector>
#include <windows.h>

#include "base/def.h"

#include "core/object.h"
#include "core/attribute.h"
#include "core/style.h"
#include "core/const.h"
#include "core/layout.h"
#include "core/context.h"
#include "core/paint.h"

#include "xml/xml.h"

namespace core
{
    extern const char* WIDGET_PROPERTY_ID;
    extern const char* WIDGET_PROPERTY_VISIBLE;
    extern const char* WIDGET_PROPERTY_TITLE;
    extern const char* WIDGET_PROPERTY_TABSTOP;
    extern const char* WIDGET_PROPERTY_ZINDEX;
    extern const char* WIDGET_PROPERTY_ALPHA;
    extern const char* WIDGET_PROPERTY_DRAG;
    extern const char* WIDGET_PROPERTY_FILTER_EVENT;



    enum Message
    {
        ST_CALCULATE,
        ST_LAYOUT,
        ST_ADJUST,
        ST_PAINT,
        ST_MOUSEHOVER,
        ST_MOUSELEAVE,
        ST_LBUTTONDOWN,
        ST_LBUTTONUP,
        ST_MOUSEWHEEL,
        ST_RBUTTONDOWN,
        ST_RBUTTONUP,
        ST_KEYDOWN,
        ST_KEYUP,
        ST_CHAR,
        ST_SHOW
    };

    #define BEGIN_MESSAGE_MAP()\
        virtual bool ProcessMessage(UINT umsg, WPARAM wparam, LPARAM lparam, LRESULT* lresult)\
        {\
            if (lresult)\
                *lresult = 0;\
            bool handled = false;\
            LRESULT result = 0;

    #define END_MESSAGE_MAP()\
                handled = false;\
            if (lresult)\
                *lresult = result;\
            return handled;\
        }

    #define MESSAGE_HANDLE(msg, func)\
        if (msg == umsg)\
        {\
            handled = true;\
            result = func(wparam, lparam, &handled);\
        }\
        else

    #define CHAIN_MESSAGE_MAP(theclass)\
        if (theclass::ProcessMessage(umsg, wparam, lparam, &result))\
        {\
            handled = true;\
        }\
        else


    class StWidget;
    class IMessageObserver
    {
    public:
        virtual bool OnMessageObserver(StWidget* widget, UINT msg, WPARAM wparam, LPARAM lparam, bool before) = 0;
    };
    
    class IClickListener
    {
    public:
        virtual bool OnClick(StWidget* widget, WPARAM wparam, LPARAM lparam) = 0;
    };

    class StWidget : public StAttributeSet
    {
    public:
        friend class StContainer;
        DECLARE_ATTRIBUTE_MAP();

        class MetaData
        {
        public:
            virtual ~MetaData() {}
            virtual void Drop() = 0;
        };

        StWidget();
        ~StWidget();

        virtual StObject* Clone();
        virtual void      Drop();

        virtual bool      IsContainer();
        virtual StWidget* HitTest(int x, int y);
        virtual void      Move(int x, int y, bool onlyChild);


        Context*            GetContext();
        virtual void        SetContext(Context* context);

        MetaData*           ExtraData();
        void                SetExtraData(MetaData* data);

        StStyle&            GetStyle(DWORD state);
        StStyle&            LayoutStyle();

        RECT                ContentRect();
        RECT                BorderRect();
        RECT                BoundaryRect();

        ILayout*            Layoutor();
        void                SetLayoutor(ILayout* layout);

        DWORD               ActionState();
        void                SetActionState(DWORD state);

        bool                IsFocused();
        void                SetFocus(bool focus);

        bool                IsCapture();
        void                SetCapture(bool capture);

        const std::string&  TagName();
        void                SetTagName(const std::string &name);

        StWidget*           Parent();
        StWidget*           PreviousSibling();
        StWidget*           NextSibling();

        void                AddMessageObserver(IMessageObserver *observer);
        void                RemoveMessageObserver(IMessageObserver *observer);
        void                NotifyObservers(UINT umsg, WPARAM wparam, LPARAM lparam, bool before);

        void                AddClickListener(IClickListener* listener);
        void                RemoveClickListener(IClickListener* listener);
        void                NotifyClickListeners(WPARAM wparam, LPARAM lparam);

        KeyWidget    Key();

        std::string  Id();
        void         SetId(const std::string& id);

        bool         IsVisible();
        void         SetVisible(bool visible);

        std::wstring Title();
        void         SetTitle(const std::wstring& title);

        bool         IsTabstop();
        void         SetTabStop(bool enable);

        bool         IsDraggable();
        void         SetDraggable(bool draggable);

        int          ZIndex();
        void         SetZIndex(int z_index);

        int          Alpha();
        void         SetAlpha(int alpha);

        bool         IsFilterEvent();
        void         SetFilterEvent(bool filter);


        LRESULT     Dispatch(UINT umsg, WPARAM wparam, LPARAM lparam);

        void        ReLayoutSelf(bool repaint = true);
        void        ReLayout(bool repaint = true);
        void        Invalidate();
        void        InvalidateRect(RECT &rect);

        void        Inherit(StWidget& widget);

        LayoutParam GetLayoutParam();

        void        MoveTo(int x, int y);

    protected:
        void CopyTo(StWidget* target);

        int  BeginPaint(StPainter *painter, RECT *clip);
        void DrawBackground(StPainter *painter);
        void DrawBorder(StPainter* painter);
        void DrawSelf(StPainter *painter);
        void EndPaint(StPainter *painter, int clip);

    protected:
        BEGIN_MESSAGE_MAP()
            MESSAGE_HANDLE(ST_CALCULATE,   OnCalculate  )
            MESSAGE_HANDLE(ST_LAYOUT,      OnLayout     )
            MESSAGE_HANDLE(ST_ADJUST,      OnAdjust     )
            MESSAGE_HANDLE(ST_PAINT,       OnPaint      )
            MESSAGE_HANDLE(ST_MOUSEHOVER,  OnMouseHover )
            MESSAGE_HANDLE(ST_MOUSELEAVE,  OnMouseLeave )
            MESSAGE_HANDLE(ST_LBUTTONDOWN, OnLButtonDown)
            MESSAGE_HANDLE(ST_LBUTTONUP,   OnLButtonUp  )
            MESSAGE_HANDLE(ST_MOUSEWHEEL,  OnMouseWheel )
            MESSAGE_HANDLE(ST_RBUTTONDOWN, OnRButtonDown)
            MESSAGE_HANDLE(ST_RBUTTONUP,   OnRButtonUp  )
            MESSAGE_HANDLE(ST_KEYDOWN,     OnKeyDown    )
            MESSAGE_HANDLE(ST_KEYUP,       OnKeyUp      )
            MESSAGE_HANDLE(ST_CHAR,        OnChar       )
            MESSAGE_HANDLE(ST_SHOW,        OnShow)
        END_MESSAGE_MAP()

        LRESULT OnCalculate   (WPARAM, LPARAM, bool*);
        LRESULT OnLayout      (WPARAM, LPARAM, bool*);
        LRESULT OnAdjust      (WPARAM, LPARAM, bool*);
        LRESULT OnPaint       (WPARAM, LPARAM, bool*);
        LRESULT OnMouseHover  (WPARAM, LPARAM, bool*);
        LRESULT OnMouseLeave  (WPARAM, LPARAM, bool*);
        LRESULT OnLButtonDown (WPARAM, LPARAM, bool*);
        LRESULT OnLButtonUp   (WPARAM, LPARAM, bool*);
        LRESULT OnMouseWheel  (WPARAM, LPARAM, bool*);
        LRESULT OnRButtonDown (WPARAM, LPARAM, bool*);
        LRESULT OnRButtonUp   (WPARAM, LPARAM, bool*);
        LRESULT OnKeyDown     (WPARAM, LPARAM, bool*);
        LRESULT OnKeyUp       (WPARAM, LPARAM, bool*);
        LRESULT OnChar        (WPARAM, LPARAM, bool*);
        LRESULT OnShow        (WPARAM, LPARAM, bool*);

    protected:
        Context*    context_;
        
        MetaData*   extra_;

        StStyle     style_normal_;
        StStyle     style_hover_;
        StStyle     style_down_;

        LayoutRect  layout_rect_;
        LayoutParam layout_param_;
        ILayout*    layoutor_;

        DWORD       action_state_;
        bool        focus_;
        bool        capture_;
        std::string tag_name_;

        StWidget*   parent_;
        StWidget*   prev_;
        StWidget*   next_;

        typedef std::vector<IMessageObserver*> MessageObservers;
        MessageObservers  observers_;

        typedef std::vector<IClickListener*> ClickListeners;
        ClickListeners    click_listeners_;

    private:
        DISABLE_COPY_AND_ASSIGN(StWidget)
    };
}

#endif