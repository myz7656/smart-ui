#ifndef __core_list_h__
#define __core_list_h__

#include "core/def.h"
#include "core/container.h"

namespace core
{
    extern const char * LIST_WIDGET_PROPERTY_SCROLL_BACKGROUND;
    extern const char * LIST_WIDGET_PROPERTY_SCROLL_BORDER;
    extern const char * LIST_WIDGET_PROPERTY_SCROLL_WIDTH;



    class StList
        : public StContainer
    {
    public:
        DECLARE_DYNAMIC_CREATE(StList);

        const static int DEFAULT_BAR_LENGTH       = 10;
        const static int DEFAULT_BAR_WIDTH        = 10;
        const static int DEFAULT_BAR_BORDER_WIDTH = 1;
        const static int DEFAULT_BAR_ALPHA        = 255;

        class Adapter
        {
        public:
            virtual ~Adapter() {}
            virtual int GetCount() = 0;
            virtual int GetHeight(int index) = 0;
            virtual int GetId(int index) = 0;
            virtual int GetType(int index) = 0;
            virtual StWidget* GetWidget(int index, StWidget* widget) = 0;
            virtual int GetTotalHeight() = 0;  ///>暂时为优化项很多情况下计算高度耗时
        };

        StList();
        ~StList();

        virtual StWidget* HitTest(int x, int y);
        virtual bool IsScroll();


        core::Color ScrollBackground();
        core::Color ScrollBorder();
        int ScrollWidth();

        void SetScrollBackground(core::Color color);
        void SetScrollBorder(core::Color color);
        void SetScrollWidth(int width);


        void SetAdapter(Adapter* adapter);
        void NotifyDataSetChanged();

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

        struct ScrollContext
        {
            int scroll_pos;

            int bar_pos;
            int bar_length;

            int cursor_pos;

            ScrollContext()
                : scroll_pos(0)
                , bar_pos(0)
                , bar_length(0)
                , cursor_pos(0) {}
        };
        struct AdapterContext
        {
            int count;
            int offset;
            int head;
            int tail;
            AdapterContext()
                : count(0)
                , offset(0)
                , head(0)
                , tail(0) {}
        };

        class Recycler
        {
        public:
            struct Cluster
            {
                int id;
                int type;
                int height;
                StWidget* widget;

                Cluster()
                    : id(0)
                    , type(0)
                    , height(0)
                    , widget(0) {}

                bool IsOK() { return widget != 0; }
            };

            Recycler();
            ~Recycler();

            void Save(int id, int type, int height, StWidget* widget);
            Cluster Recycle(int id, bool exact, bool upward);
            void Clear();

            void Insert(Recycler& recycler);

        private:
            std::vector<Cluster> clusters_;
        };

        int GetTotalHeight();
        int GetRealHeight();

        RECT GetScrollRect();
        bool IsPtInScroll(int x, int y);
        void DrawScroll(StPainter* painter);

        void UpdateScrollBarSizeAndPosition();
        void SetScrollViewPosition(int position);

        void Adjust(bool upward);

        AdapterContext GetAdapterContext();

    protected:
        Adapter* adapter_;

        ScrollContext  scroll_context_;

        Recycler recycler_;

    private:
        DISABLE_COPY_AND_ASSIGN(StList)
    };
}

#endif