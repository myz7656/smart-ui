#ifndef __core_controls_img_h__
#define __core_controls_img_h__

#include "core/def.h"
#include "core/container.h"

#include <string>

namespace core
{
    extern const char * IMAGE_WIDGET_PROPERTY_SOURCE;
    extern const char * GRID_IMAGE_WIDGET_PROPERTY_GRID;
    extern const char * GRID_IMAGE_WIDGET_PROPERTY_LEFT;
    extern const char * GRID_IMAGE_WIDGET_PROPERTY_TOP;
    extern const char * GRID_IMAGE_WIDGET_PROPERTY_RIGHT;
    extern const char * GRID_IMAGE_WIDGET_PROPERTY_BOTTOM;
    extern const char * FRAME_IMAGE_WIDGET_PROPERTY_LENGTH;
    extern const char * FRAME_IMAGE_WIDGET_PROPERTY_INDEX;



    class StImage : public StContainer
    {
    public:
        DECLARE_DYNAMIC_CREATE(StImage)

        StImage();
        ~StImage();

        virtual StObject* Clone();

        std::wstring GetSource();
        void SetSource(const std::wstring& file);

    protected:
        DECLARE_ATTRIBUTE_MAP()

        BEGIN_MESSAGE_MAP()
            MESSAGE_HANDLE(ST_CALCULATE, OnCalculate)
            MESSAGE_HANDLE(ST_PAINT, OnPaint)
            CHAIN_MESSAGE_MAP(StContainer)
        END_MESSAGE_MAP()

        LRESULT OnCalculate(WPARAM, LPARAM, bool*);
        LRESULT OnPaint(WPARAM, LPARAM, bool*);

        void CopyTo(StImage* image);

    private:
        DISABLE_COPY_AND_ASSIGN(StImage)
    };



    class StGridImage : public StImage
    {
    public:
        DECLARE_DYNAMIC_CREATE(StGridImage)

        StGridImage();
        ~StGridImage();

        int GridLeft();
        int GridTop();
        int GridRight();
        int GridBottom();

        void SetGridLeft(int left);
        void SetGridTop(int top);
        void SetGridRight(int right);
        void SetGridBottom(int bottom);

    protected:
        DECLARE_ATTRIBUTE_MAP()

        BEGIN_MESSAGE_MAP()
            MESSAGE_HANDLE(ST_PAINT, OnPaint)
            CHAIN_MESSAGE_MAP(StImage)
        END_MESSAGE_MAP()

        LRESULT OnPaint(WPARAM, LPARAM, bool*);

        bool ParseGrid(const char * p);

    private:
        DISABLE_COPY_AND_ASSIGN(StGridImage)
    };



    class StFrameGridImage : public StGridImage
    {
    public:
        DECLARE_DYNAMIC_CREATE(StFrameGridImage)

        StFrameGridImage();
        ~StFrameGridImage();

        int FrameLength();
        void SetFrameLength(int length);

        int FrameIndex();
        void SetFrameIndex(int index);

    protected:
        DECLARE_ATTRIBUTE_MAP()

        BEGIN_MESSAGE_MAP()
            MESSAGE_HANDLE(ST_CALCULATE, OnCalculate)
            MESSAGE_HANDLE(ST_PAINT, OnPaint)
            CHAIN_MESSAGE_MAP(StGridImage)
        END_MESSAGE_MAP()

        LRESULT OnCalculate(WPARAM, LPARAM, bool*);
        LRESULT OnPaint(WPARAM, LPARAM, bool*);

    private:
        DISABLE_COPY_AND_ASSIGN(StFrameGridImage)
    };
};
#endif