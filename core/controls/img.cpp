#include "core/controls/img.h"

#include "win/gdi_painter.h"

namespace core
{
    const char * IMAGE_WIDGET_PROPERTY_SOURCE              = "src";
    const char * GRID_IMAGE_WIDGET_PROPERTY_GRID           = "grid";
    const char * GRID_IMAGE_WIDGET_PROPERTY_LEFT           = "grid-left";
    const char * GRID_IMAGE_WIDGET_PROPERTY_TOP            = "grid-top";
    const char * GRID_IMAGE_WIDGET_PROPERTY_RIGHT          = "grid-right";
    const char * GRID_IMAGE_WIDGET_PROPERTY_BOTTOM         = "grid-bottom";
    const char * FRAME_IMAGE_WIDGET_PROPERTY_LENGTH        = "frame-length";
    const char * FRAME_IMAGE_WIDGET_PROPERTY_INDEX         = "frame-index";
    


    IMPLY_DYNAMIC_CREATE(StImage,           "img")
    IMPLY_DYNAMIC_CREATE(StGridImage,       "grid-img")
    IMPLY_DYNAMIC_CREATE(StFrameGridImage,  "frame-grid-img")



    BEGIN_ATTRIBUTE_MAP(StImage)
        ATTRIBUTE(IMAGE_WIDGET_PROPERTY_SOURCE, StAttributeFile, ATTRIBUTE_TRAIT_BOTH)
        CHAIN_ATTRIBUTE_MAP(StContainer)
    END_ATTRIBUTE_MAP()

    StImage::StImage() {}

    StImage::~StImage() {}

    StObject* StImage::Clone()
    {
        StImage* image = new StImage;
        if (!image)
        {
            return 0;
        }

        CopyTo(image);
        return image;
    }

    std::wstring StImage::GetSource()
    {
        std::wstring file;
        GetAttribute(IMAGE_WIDGET_PROPERTY_SOURCE, &file);
        return file;
    }

    void StImage::SetSource(const std::wstring& file)
    {
        AddAttribute(IMAGE_WIDGET_PROPERTY_SOURCE, file);
    }

    LRESULT StImage::OnCalculate(WPARAM wparam, LPARAM lparam, bool* handled)
    {
        if (!StContainer::OnCalculate(wparam, lparam, handled))
        {
            return FALSE;
        }

        BoxParam* box_param = (BoxParam*)lparam;
        std::wstring file = GetSource();
        if (file.empty())
        {
            return FALSE;
        }

        int width = 0;
        int height = 0;

        StStyle& style = LayoutStyle();
        if (!style.HasAttribute(STYLE_PROPERTY_WIDTH) ||
            !style.HasAttribute(STYLE_PROPERTY_HEIGHT))
        {
            win::WinPainter painter(0);
            painter.CalculateImage(file, &width, &height);
        }

        if (!style.HasAttribute(STYLE_PROPERTY_WIDTH))
        {
            box_param->width = width;
        }
        if (!style.HasAttribute(STYLE_PROPERTY_HEIGHT))
        {
            box_param->height = height;
        }

        return TRUE;
    }

    LRESULT StImage::OnPaint(WPARAM wparam, LPARAM lparam, bool*)
    {
        if (!IsVisible())
        {
            return FALSE;
        }

        StPainter* painter = (StPainter*)wparam;
        RECT* clip = (RECT*)lparam;

        int nclip = BeginPaint(painter, clip);
        if (nclip == 0)
        {
            return FALSE;
        }

        DrawSelf(painter);

        int alpha = Alpha();
        std::wstring file = GetSource();
        if (!file.empty())
        {
            painter->DrawImage(file, layout_rect_.border, alpha);
        }

        DrawChild(painter, clip);

        EndPaint(painter, nclip);

        return TRUE;
    }

    void StImage::CopyTo( StImage* image )
    {
        StContainer::CopyTo(image);
    }



    BEGIN_ATTRIBUTE_MAP(StGridImage)
        ATTRIBUTE_PARSER(GRID_IMAGE_WIDGET_PROPERTY_GRID, ParseGrid, ATTRIBUTE_TRAIT_PAINT)
        ATTRIBUTE(GRID_IMAGE_WIDGET_PROPERTY_TOP, StAttributeInteger, ATTRIBUTE_TRAIT_PAINT)
        ATTRIBUTE(GRID_IMAGE_WIDGET_PROPERTY_LEFT, StAttributeInteger, ATTRIBUTE_TRAIT_PAINT)
        ATTRIBUTE(GRID_IMAGE_WIDGET_PROPERTY_BOTTOM, StAttributeInteger, ATTRIBUTE_TRAIT_PAINT)
        ATTRIBUTE(GRID_IMAGE_WIDGET_PROPERTY_RIGHT, StAttributeInteger, ATTRIBUTE_TRAIT_PAINT)
        CHAIN_ATTRIBUTE_MAP(StImage)
    END_ATTRIBUTE_MAP()

    StGridImage::StGridImage() {}

    StGridImage::~StGridImage() {}

    int StGridImage::GridLeft()
    {
        int grid_left = 0;
        GetAttribute(GRID_IMAGE_WIDGET_PROPERTY_LEFT, &grid_left, 0);
        return grid_left;
    }

    int StGridImage::GridTop()
    {
        int grid_top = 0;
        GetAttribute(GRID_IMAGE_WIDGET_PROPERTY_TOP, &grid_top, 0);
        return grid_top;
    }

    int StGridImage::GridRight()
    {
        int grid_right = 0;
        GetAttribute(GRID_IMAGE_WIDGET_PROPERTY_RIGHT, &grid_right, 0);
        return grid_right;
    }

    int StGridImage::GridBottom()
    {
        int grid_bottom = 0;
        GetAttribute(GRID_IMAGE_WIDGET_PROPERTY_BOTTOM, &grid_bottom, 0);
        return grid_bottom;
    }

    void StGridImage::SetGridLeft(int left)
    {
        AddAttribute(GRID_IMAGE_WIDGET_PROPERTY_LEFT, left);
    }

    void StGridImage::SetGridTop(int top)
    {
        AddAttribute(GRID_IMAGE_WIDGET_PROPERTY_TOP, top);
    }

    void StGridImage::SetGridRight(int right)
    {
        AddAttribute(GRID_IMAGE_WIDGET_PROPERTY_RIGHT, right);
    }

    void StGridImage::SetGridBottom(int bottom)
    {
        AddAttribute(GRID_IMAGE_WIDGET_PROPERTY_BOTTOM, bottom);
    }

    LRESULT StGridImage::OnPaint(WPARAM wparam, LPARAM lparam, bool*)
    {
        if (!IsVisible())
        {
            return FALSE;
        }

        StPainter* painter = (StPainter*)wparam;
        RECT* clip = (RECT*)lparam;

        int nclip = BeginPaint(painter, clip);
        if (nclip == 0)
        {
            return FALSE;
        }

        DrawSelf(painter);

        int alpha = Alpha();
        std::wstring file = GetSource();
        if (!file.empty())
        {
            int grid_left = GridLeft();
            int grid_top = GridTop();
            int grid_right = GridRight();
            int grid_bottom = GridBottom();

            RECT grid = { grid_left, grid_top, grid_right, grid_bottom };
            painter->DrawGridImage(file, layout_rect_.border, grid, alpha);
        }

        DrawChild(painter, clip);

        EndPaint(painter, nclip);

        return TRUE;
    }

    bool StGridImage::ParseGrid(const char * p)
    {
        int value = StAttributeInteger::Parse(p);
        AddAttribute(GRID_IMAGE_WIDGET_PROPERTY_TOP, value);
        AddAttribute(GRID_IMAGE_WIDGET_PROPERTY_LEFT, value);
        AddAttribute(GRID_IMAGE_WIDGET_PROPERTY_BOTTOM, value);
        AddAttribute(GRID_IMAGE_WIDGET_PROPERTY_RIGHT, value);
        return true;
    }



    BEGIN_ATTRIBUTE_MAP(StFrameGridImage)
        ATTRIBUTE(FRAME_IMAGE_WIDGET_PROPERTY_LENGTH, StAttributeInteger, ATTRIBUTE_TRAIT_BOTH)
        ATTRIBUTE(FRAME_IMAGE_WIDGET_PROPERTY_INDEX, StAttributeInteger, ATTRIBUTE_TRAIT_PAINT)
        CHAIN_ATTRIBUTE_MAP(StGridImage)
    END_ATTRIBUTE_MAP()

    StFrameGridImage::StFrameGridImage() {}

    StFrameGridImage::~StFrameGridImage() {}

    int StFrameGridImage::FrameLength()
    {
        int frame_length = 0;
        GetAttribute(FRAME_IMAGE_WIDGET_PROPERTY_LENGTH, &frame_length, 0);
        return frame_length;
    }

    void StFrameGridImage::SetFrameLength(int length)
    {
        AddAttribute(FRAME_IMAGE_WIDGET_PROPERTY_LENGTH, length);
    }

    int StFrameGridImage::FrameIndex()
    {
        int frame_index = 0;
        GetAttribute(FRAME_IMAGE_WIDGET_PROPERTY_INDEX, &frame_index, 0);
        return frame_index;
    }

    void StFrameGridImage::SetFrameIndex(int index)
    {
        AddAttribute(FRAME_IMAGE_WIDGET_PROPERTY_INDEX, index);
    }

    LRESULT StFrameGridImage::OnCalculate(WPARAM wparam, LPARAM lparam, bool* handled)
    {
        if (!StGridImage::OnCalculate(wparam, lparam, handled))
        {
            return FALSE;
        }

        BoxParam* box_param = (BoxParam*)lparam;
        if (HasAttribute(FRAME_IMAGE_WIDGET_PROPERTY_LENGTH))
        {
            box_param->width = FrameLength();
        }

        return TRUE;
    }

    LRESULT StFrameGridImage::OnPaint(WPARAM wparam, LPARAM lparam, bool*)
    {
        if (!IsVisible())
        {
            return FALSE;
        }

        StPainter* painter = (StPainter*)wparam;
        RECT* clip = (RECT*)lparam;

        int nclip = BeginPaint(painter, clip);
        if (nclip == 0)
        {
            return FALSE;
        }

        DrawSelf(painter);

        int alpha = Alpha();
        std::wstring file = GetSource();
        if (!file.empty())
        {
            int grid_left = GridLeft();
            int grid_top = GridTop();
            int grid_right = GridRight();
            int grid_bottom = GridBottom();

            int frame_length = FrameLength();
            int frame_index = FrameIndex();

            RECT rect = { grid_left, grid_top, grid_right, grid_bottom };
            painter->DrawFrameGridImage(file, layout_rect_.border, rect, frame_length, frame_index, alpha);
        }

        DrawChild(painter, clip);

        EndPaint(painter, nclip);

        return TRUE;
    }
};