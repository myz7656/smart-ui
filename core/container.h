#ifndef __core_container_h__
#define __core_container_h__

#include "core/widget.h"

namespace core
{
    class StContainer : public StWidget
    {
    public:
        StContainer();
        ~StContainer();

        virtual StObject* Clone();
        virtual void      Drop();

        virtual bool      IsContainer();
        virtual StWidget* HitTest(int x, int y);
        virtual void      Move(int x, int y, bool onlyChild);
        virtual void      SetContext(Context* context);
        virtual bool      IsFilterWidget(const char* name);
        virtual bool      IsScroll();


        StWidget* FindWidget(const std::string& id);

        StWidget* FirstChild();
        StWidget* LastChild();
        StWidget* FirstChild(const std::string& id);
        StWidget* LastChild (const std::string& id);

        void LinkBegin(StWidget *add);
        void LinkEnd(StWidget *add);
        void InsertBefore(StWidget *add, StWidget *before);
        void InsertAfter(StWidget *add, StWidget *after);
        void Replace(StWidget *with, StWidget *replace);
        void Remove(StWidget *remove);
        StWidget* Accept();
        void Clear();

    protected:
        void CopyTo(StContainer* container);

        void DrawChild(StPainter *painter, RECT *clip);

    protected:
        BEGIN_MESSAGE_MAP()
            MESSAGE_HANDLE(ST_LAYOUT, OnLayout)
            MESSAGE_HANDLE(ST_PAINT, OnPaint)
            CHAIN_MESSAGE_MAP(StWidget)
        END_MESSAGE_MAP()

        LRESULT OnLayout(WPARAM, LPARAM, bool*);
        LRESULT OnPaint(WPARAM, LPARAM, bool*);

    protected:
        StWidget *first_;
        StWidget *last_;

    private:
        DISABLE_COPY_AND_ASSIGN(StContainer)
    };
}

#endif