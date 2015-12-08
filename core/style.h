#ifndef __core_style_h__
#define __core_style_h__

#include "base/def.h"

#include "core/attribute.h"
#include "core/const.h"


namespace core
{
    extern const char* STYLE_PROPERTY_BACKGROUND;
    extern const char* STYLE_PROPERTY_BACKGROUND_COLOR;
    extern const char* STYLE_PROPERTY_BACKGROUND_IMAGE;
    extern const char* STYLE_PROPERTY_BACKGROUND_REPEAT;
    extern const char* STYLE_PROPERTY_COLOR;
    extern const char* STYLE_PROPERTY_LINE_HEIGHT;
    extern const char* STYLE_PROPERTY_HORIZONTAL_ALIGN;
    extern const char* STYLE_PROPERTY_VERTICAL_ALIGN;
    extern const char* STYLE_PROPERTY_TEXT_ALIGN;
    extern const char* STYLE_PROPERTY_UNDERLINE;
    extern const char* STYLE_PROPERTY_TEXT_INDENT;
    extern const char* STYLE_PROPERTY_FONT;
    extern const char* STYLE_PROPERTY_FONT_FAMILY;
    extern const char* STYLE_PROPERTY_FONT_SIZE;
    extern const char* STYLE_PROPERTY_FONT_STYLE;
    extern const char* STYLE_PROPERTY_BORDER;
    extern const char* STYLE_PROPERTY_BORDER_WIDTH;
    extern const char* STYLE_PROPERTY_BORDER_STYLE;
    extern const char* STYLE_PROPERTY_BORDER_COLOR;
    extern const char* STYLE_PROPERTY_POSITION;
    extern const char* STYLE_PROPERTY_MARGIN;
    extern const char* STYLE_PROPERTY_MARGIN_LEFT;
    extern const char* STYLE_PROPERTY_MARGIN_TOP;
    extern const char* STYLE_PROPERTY_MARGIN_RIGHT;
    extern const char* STYLE_PROPERTY_MARGIN_BOTTOM;
    extern const char* STYLE_PROPERTY_PADDING;
    extern const char* STYLE_PROPERTY_PADDING_LEFT;
    extern const char* STYLE_PROPERTY_PADDING_TOP;
    extern const char* STYLE_PROPERTY_PADDING_RIGHT;
    extern const char* STYLE_PROPERTY_PADDING_BOTTOM;
    extern const char* STYLE_PROPERTY_LEFT;
    extern const char* STYLE_PROPERTY_TOP;
    extern const char* STYLE_PROPERTY_RIGHT;
    extern const char* STYLE_PROPERTY_BOTTOM;
    extern const char* STYLE_PROPERTY_WIDTH;
    extern const char* STYLE_PROPERTY_HEIGHT;
    extern const char* STYLE_PROPERTY_CURSOR;
    extern const char* STYLE_PROPERTY_BOX_MODE;
    extern const char* STYLE_PROPERTY_STROKE_WIDTH;
    extern const char* STYLE_PROPERTY_BORDER_RADIUS;
    extern const char* STYLE_PROPERTY_BORDER_RADIUS_WIDTH;
    extern const char* STYLE_PROPERTY_BORDER_RADIUS_HEIGHT;



    extern const wchar_t* DEFAULT_FONT_FAMILY;



    class StStyle : public StAttributeSet
    {
    public:
        DECLARE_ATTRIBUTE_MAP();

        enum StyleType
        {
            STYLE_TAG,
            STYLE_CLASS,
            STYLE_IDENTIFIER
        };

        StStyle();
        StStyle(const StStyle&);
        ~StStyle();
        StStyle& operator=(const StStyle&);

        StObject* Clone();
        void Drop();


        StyleType          Type() const                  { return type_; }
        void               SetType(StyleType type)       { type_ = type; }

        const std::string& Name() const                  { return name_; }
        void               SetName(const std::string &s) { name_ = s; }

        const DWORD        State() const                 { return state_; }
        void               SetState(DWORD state)         { state_ = state; }


        core::Color     BackgroundColor();
        std::wstring    BackgroundImage();
        DWORD           BackgroundRepeat();
        core::Color     TextColor();
        int             LineHeight();
        DWORD           HorizontalAlignment();
        DWORD           VerticalAlignment();
        DWORD           TextAlignment();
        bool            IsUnderline();
        int             TextIndent();
        std::wstring    FontFamily();
        core::Unit      FontSize();
        DWORD           FontStyle();
        int             BorderWidth();
        DWORD           BorderStyle();
        core::Color     BorderColor();
        DWORD           Position();
        core::Unit      MarginLeft();
        core::Unit      MarginTop();
        core::Unit      MarginRight();
        core::Unit      MarginBottom();
        core::Unit      PaddingLeft();
        core::Unit      PaddingTop();
        core::Unit      PaddingRight();
        core::Unit      PaddingBottom();
        core::Unit      Left();
        core::Unit      Top();
        core::Unit      Right();
        core::Unit      Bottom();
        core::Unit      Width();
        core::Unit      Height();
        DWORD           CursorType();
        DWORD           BoxMode();
        int             BorderRadiusWidth();
        int             BorderRadiusHeight();

        void            SetBackgroundColor(const core::Color&);
        void            SetBackgroundImage(const std::wstring&);
        void            SetBackgroundRepeat(DWORD);
        void            SetTextColor(const core::Color&);
        void            SetLineHeight(int);
        void            SetHorizontalAlignment(DWORD);
        void            SetVerticalAlignment(DWORD);
        void            SetTextAlignment(DWORD);
        void            SetUnderline(bool);
        void            SetTextIndent(int);
        void            SetFontFamily(const std::wstring&);
        void            SetFontSize(const core::Unit&);
        void            SetFontStyle(DWORD);
        void            SetBorderWidth(int);
        void            SetBorderStyle(DWORD);
        void            SetBorderColor(const core::Color&);
        void            SetPosition(DWORD);
        void            SetMarginLeft(const core::Unit&);
        void            SetMarginTop(const core::Unit&);
        void            SetMarginRight(const core::Unit&);
        void            SetMarginBottom(const core::Unit&);
        void            SetPaddingLeft(const core::Unit&);
        void            SetPaddingTop(const core::Unit&);
        void            SetPaddingRight(const core::Unit&);
        void            SetPaddingBottom(const core::Unit&);
        void            SetLeft(const core::Unit&);
        void            SetTop(const core::Unit&);
        void            SetRight(const core::Unit&);
        void            SetBottom(const core::Unit&);
        void            SetWidth(const core::Unit&);
        void            SetHeight(const core::Unit&);
        void            SetCursor(DWORD);
        void            SetBoxMode(DWORD);
        void            SetBorderRadiusWidth(int);
        void            SetBorderRadiusHeight(int);

        const char * ParseValue (const char * p);
        void         ParseValues(const char * p);

        void Copy(const StStyle& style);
        void Inherit(const StStyle& style);

    private:
        bool ParseBackground(const char *p);
        bool ParseMargin(const char *p);
        bool ParsePadding(const char *p);
        bool ParseFont(const char *p);
        bool ParseBorder(const char *p);
        bool ParserBorderRadius(const char *p);

    private:
        StyleType   type_;
        DWORD       state_;
        std::string name_;
    };
}

#endif