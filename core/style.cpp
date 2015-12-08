#include "core/style.h"
#include "util/tstring.h"

namespace core
{
    const char* STYLE_PROPERTY_BACKGROUND               = "background";
    const char* STYLE_PROPERTY_BACKGROUND_COLOR         = "background-color";
    const char* STYLE_PROPERTY_BACKGROUND_IMAGE         = "background-image";
    const char* STYLE_PROPERTY_BACKGROUND_REPEAT        = "background-repeat";
    const char* STYLE_PROPERTY_COLOR                    = "color";
    const char* STYLE_PROPERTY_LINE_HEIGHT              = "line-height";
    const char* STYLE_PROPERTY_HORIZONTAL_ALIGN         = "h-align";
    const char* STYLE_PROPERTY_VERTICAL_ALIGN           = "v-align";
    const char* STYLE_PROPERTY_TEXT_ALIGN               = "text-align";
    const char* STYLE_PROPERTY_UNDERLINE                = "underline";
    const char* STYLE_PROPERTY_TEXT_INDENT              = "text-indent";
    const char* STYLE_PROPERTY_FONT                     = "font";
    const char* STYLE_PROPERTY_FONT_FAMILY              = "font-family";
    const char* STYLE_PROPERTY_FONT_SIZE                = "font-size";
    const char* STYLE_PROPERTY_FONT_STYLE               = "font-style";
    const char* STYLE_PROPERTY_BORDER                   = "border";
    const char* STYLE_PROPERTY_BORDER_WIDTH             = "border-width";
    const char* STYLE_PROPERTY_BORDER_STYLE             = "border-style";
    const char* STYLE_PROPERTY_BORDER_COLOR             = "border-color";
    const char* STYLE_PROPERTY_POSITION                 = "position";
    const char* STYLE_PROPERTY_MARGIN                   = "margin";
    const char* STYLE_PROPERTY_MARGIN_LEFT              = "margin-left";
    const char* STYLE_PROPERTY_MARGIN_TOP               = "margin-top";
    const char* STYLE_PROPERTY_MARGIN_RIGHT             = "margin-right";
    const char* STYLE_PROPERTY_MARGIN_BOTTOM            = "margin-bottom";
    const char* STYLE_PROPERTY_PADDING                  = "padding";
    const char* STYLE_PROPERTY_PADDING_LEFT             = "padding-left";
    const char* STYLE_PROPERTY_PADDING_TOP              = "padding-top";
    const char* STYLE_PROPERTY_PADDING_RIGHT            = "padding-right";
    const char* STYLE_PROPERTY_PADDING_BOTTOM           = "padding-bottom";
    const char* STYLE_PROPERTY_LEFT                     = "left";
    const char* STYLE_PROPERTY_TOP                      = "top";
    const char* STYLE_PROPERTY_RIGHT                    = "right";
    const char* STYLE_PROPERTY_BOTTOM                   = "bottom";
    const char* STYLE_PROPERTY_WIDTH                    = "width";
    const char* STYLE_PROPERTY_HEIGHT                   = "height";
    const char* STYLE_PROPERTY_CURSOR                   = "cursor";
    const char* STYLE_PROPERTY_BOX_MODE                 = "box-mode";
    const char* STYLE_PROPERTY_STROKE_WIDTH             = "stroke-width";
    const char* STYLE_PROPERTY_BORDER_RADIUS            = "border-radius";
    const char* STYLE_PROPERTY_BORDER_RADIUS_WIDTH      = "border-radius-width";
    const char* STYLE_PROPERTY_BORDER_RADIUS_HEIGHT     = "border-radius-height";

    const wchar_t* DEFAULT_FONT_FAMILY                  = L"Î¢ÈíÑÅºÚ";



    BEGIN_ATTRIBUTE_MAP(StStyle)
        ATTRIBUTE_PARSER(STYLE_PROPERTY_BACKGROUND, ParseBackground, ATTRIBUTE_TRAIT_PAINT)
        ATTRIBUTE(STYLE_PROPERTY_BACKGROUND_COLOR, StAttributeColor, ATTRIBUTE_TRAIT_PAINT)
        ATTRIBUTE(STYLE_PROPERTY_BACKGROUND_IMAGE, StAttributeFile, ATTRIBUTE_TRAIT_PAINT)

        ATTRIBUTE_ENUM_BEGIN(STYLE_PROPERTY_BACKGROUND_REPEAT, StAttributeDword)
            ATTRIBUTE_ENUM_VALUE("repeat-x", BACKGROUND_REPEAT_X)
            ATTRIBUTE_ENUM_VALUE("repeat-y", BACKGROUND_REPEAT_Y)
            ATTRIBUTE_ENUM_VALUE("repeat", BACKGROUND_REPEAT_X | BACKGROUND_REPEAT_Y)
            ATTRIBUTE_ENUM_VALUE("no-repeat", BACKGROUND_REPEAT_NULL)
        ATTRIBUTE_ENUM_END(ATTRIBUTE_TRAIT_PAINT)

        ATTRIBUTE(STYLE_PROPERTY_COLOR, StAttributeColor, ATTRIBUTE_TRAIT_PAINT)
        ATTRIBUTE(STYLE_PROPERTY_LINE_HEIGHT, StAttributeInteger, ATTRIBUTE_TRAIT_BOTH)

        ATTRIBUTE_ENUM_BEGIN(STYLE_PROPERTY_HORIZONTAL_ALIGN, StAttributeDword)
            ATTRIBUTE_ENUM_VALUE("left", HORIZONTAL_ALIGNMENT_LEFT)
            ATTRIBUTE_ENUM_VALUE("center", HORIZONTAL_ALIGNMENT_CENTER)
            ATTRIBUTE_ENUM_VALUE("right", HORIZONTAL_ALIGNMENT_RIGHT)
        ATTRIBUTE_ENUM_END(ATTRIBUTE_TRAIT_BOTH)

        ATTRIBUTE_ENUM_BEGIN(STYLE_PROPERTY_VERTICAL_ALIGN, StAttributeDword)
            ATTRIBUTE_ENUM_VALUE("top", VERTICAL_ALIGNMENT_TOP)
            ATTRIBUTE_ENUM_VALUE("middle", VERTICAL_ALIGNMENT_MIDDLE)
            ATTRIBUTE_ENUM_VALUE("bottom", VERTICAL_ALIGNMENT_BOTTOM)
        ATTRIBUTE_ENUM_END(ATTRIBUTE_TRAIT_BOTH)

        ATTRIBUTE_ENUM_BEGIN(STYLE_PROPERTY_TEXT_ALIGN, StAttributeDword)
            ATTRIBUTE_ENUM_VALUE("top", VERTICAL_ALIGNMENT_TOP)
            ATTRIBUTE_ENUM_VALUE("middle", VERTICAL_ALIGNMENT_MIDDLE)
            ATTRIBUTE_ENUM_VALUE("bottom", VERTICAL_ALIGNMENT_BOTTOM)
        ATTRIBUTE_ENUM_END(ATTRIBUTE_TRAIT_BOTH)

        ATTRIBUTE_ENUM_BEGIN(STYLE_PROPERTY_UNDERLINE, StAttributeBool)
            ATTRIBUTE_ENUM_VALUE("underline", true)
            ATTRIBUTE_ENUM_VALUE("none", false)
        ATTRIBUTE_ENUM_END(ATTRIBUTE_TRAIT_PAINT)

        ATTRIBUTE(STYLE_PROPERTY_TEXT_INDENT, StAttributeInteger, ATTRIBUTE_TRAIT_BOTH)

        ATTRIBUTE_PARSER(STYLE_PROPERTY_FONT, ParseFont, ATTRIBUTE_TRAIT_BOTH)

        ATTRIBUTE(STYLE_PROPERTY_FONT_FAMILY, StAttributeWString, ATTRIBUTE_TRAIT_BOTH)
        ATTRIBUTE(STYLE_PROPERTY_FONT_SIZE, StAttributeUnit, ATTRIBUTE_TRAIT_BOTH)

        ATTRIBUTE_ENUM_BEGIN(STYLE_PROPERTY_FONT_STYLE, StAttributeDword)
            ATTRIBUTE_ENUM_VALUE("normal", FONT_STYLE_NORMAL)
            ATTRIBUTE_ENUM_VALUE("italic", FONT_STYLE_ITALIC)
            ATTRIBUTE_ENUM_VALUE("bold", FONT_STYLE_BOLD)
            ATTRIBUTE_ENUM_VALUE("mix", FONT_STYLE_BOLD_ITALIC)
        ATTRIBUTE_ENUM_END(ATTRIBUTE_TRAIT_BOTH)

        ATTRIBUTE_PARSER(STYLE_PROPERTY_BORDER, ParseBorder, ATTRIBUTE_TRAIT_BOTH)

        ATTRIBUTE(STYLE_PROPERTY_BORDER_WIDTH, StAttributeInteger, ATTRIBUTE_TRAIT_BOTH)

        ATTRIBUTE_ENUM_BEGIN(STYLE_PROPERTY_BORDER_STYLE, StAttributeDword)
            ATTRIBUTE_ENUM_VALUE("solid", BORDER_STYLE_SOLID)
            ATTRIBUTE_ENUM_VALUE("dashed", BORDER_STYLE_DASHED)
            ATTRIBUTE_ENUM_VALUE("dotted", BORDER_STYLE_DOTTED)
        ATTRIBUTE_ENUM_END(ATTRIBUTE_TRAIT_PAINT)

        ATTRIBUTE(STYLE_PROPERTY_BORDER_COLOR, StAttributeColor, ATTRIBUTE_TRAIT_PAINT)

        ATTRIBUTE_ENUM_BEGIN(STYLE_PROPERTY_POSITION, StAttributeDword)
            ATTRIBUTE_ENUM_VALUE("absolute", POSITION_ABSOLUTE)
            ATTRIBUTE_ENUM_VALUE("relative", POSITION_RELATIVE)
        ATTRIBUTE_ENUM_END(ATTRIBUTE_TRAIT_BOTH)

        ATTRIBUTE_PARSER(STYLE_PROPERTY_MARGIN, ParseMargin, ATTRIBUTE_TRAIT_BOTH)
        ATTRIBUTE(STYLE_PROPERTY_MARGIN_LEFT, StAttributeUnit, ATTRIBUTE_TRAIT_BOTH)
        ATTRIBUTE(STYLE_PROPERTY_MARGIN_TOP, StAttributeUnit, ATTRIBUTE_TRAIT_BOTH)
        ATTRIBUTE(STYLE_PROPERTY_MARGIN_RIGHT, StAttributeUnit, ATTRIBUTE_TRAIT_BOTH)
        ATTRIBUTE(STYLE_PROPERTY_MARGIN_BOTTOM, StAttributeUnit, ATTRIBUTE_TRAIT_BOTH)

        ATTRIBUTE_PARSER(STYLE_PROPERTY_PADDING, ParsePadding, ATTRIBUTE_TRAIT_BOTH)
        ATTRIBUTE(STYLE_PROPERTY_PADDING_LEFT, StAttributeUnit, ATTRIBUTE_TRAIT_BOTH)
        ATTRIBUTE(STYLE_PROPERTY_PADDING_TOP, StAttributeUnit, ATTRIBUTE_TRAIT_BOTH)
        ATTRIBUTE(STYLE_PROPERTY_PADDING_RIGHT, StAttributeUnit, ATTRIBUTE_TRAIT_BOTH)
        ATTRIBUTE(STYLE_PROPERTY_PADDING_BOTTOM, StAttributeUnit, ATTRIBUTE_TRAIT_BOTH)

        ATTRIBUTE(STYLE_PROPERTY_LEFT, StAttributeUnit, ATTRIBUTE_TRAIT_BOTH)
        ATTRIBUTE(STYLE_PROPERTY_TOP, StAttributeUnit, ATTRIBUTE_TRAIT_BOTH)
        ATTRIBUTE(STYLE_PROPERTY_RIGHT, StAttributeUnit, ATTRIBUTE_TRAIT_BOTH)
        ATTRIBUTE(STYLE_PROPERTY_BOTTOM, StAttributeUnit, ATTRIBUTE_TRAIT_BOTH)

        ATTRIBUTE(STYLE_PROPERTY_WIDTH, StAttributeUnit, ATTRIBUTE_TRAIT_BOTH)
        ATTRIBUTE(STYLE_PROPERTY_HEIGHT, StAttributeUnit, ATTRIBUTE_TRAIT_BOTH)

        ATTRIBUTE_ENUM_BEGIN(STYLE_PROPERTY_CURSOR, StAttributeDword)
            ATTRIBUTE_ENUM_VALUE("hand", CURSOR_TYPE_HAND)
            ATTRIBUTE_ENUM_VALUE("arrow", CURSOR_TYPE_ARROW)
        ATTRIBUTE_ENUM_END(ATTRIBUTE_TRAIT_NULL)

        ATTRIBUTE_ENUM_BEGIN(STYLE_PROPERTY_BOX_MODE, StAttributeDword)
            ATTRIBUTE_ENUM_VALUE("classic", BOX_MODE_CLASSIC)
            ATTRIBUTE_ENUM_VALUE("modern", BOX_MODE_MODERN)
        ATTRIBUTE_ENUM_END(ATTRIBUTE_TRAIT_BOTH)

        ATTRIBUTE_PARSER(STYLE_PROPERTY_BORDER_RADIUS, ParserBorderRadius, ATTRIBUTE_TRAIT_BOTH)
        ATTRIBUTE(STYLE_PROPERTY_BORDER_RADIUS_WIDTH, StAttributeInteger, ATTRIBUTE_TRAIT_BOTH)
        ATTRIBUTE(STYLE_PROPERTY_BORDER_RADIUS_HEIGHT, StAttributeInteger, ATTRIBUTE_TRAIT_BOTH)
    END_ATTRIBUTE_MAP()

    StStyle::StStyle() {}

    StStyle::StStyle(const StStyle& style)
        : StAttributeSet(style)
    {
        type_ = style.type_;
        state_ = style.state_;
        name_ = style.name_;
    }

    StStyle::~StStyle() {}

    StStyle& StStyle::operator=(const StStyle& style)
    {
        if (this == &style)
        {
            return *this;
        }
        StAttributeSet::operator=(style);

        type_ = style.type_;
        state_ = style.state_;
        name_ = style.name_;

        return *this;
    }


    StObject* StStyle::Clone()
    {
        StStyle* style = new StStyle(*this);
        if (!style)
        {
            return 0;
        }
        return static_cast<StObject*>(style);
    }

    void StStyle::Drop()
    {
        delete this;
    }


    core::Color StStyle::BackgroundColor()
    {
        core::Color color;
        GetAttribute(STYLE_PROPERTY_BACKGROUND_COLOR, &color);
        return color;
    }

    std::wstring StStyle::BackgroundImage()
    {
        std::wstring image;
        GetAttribute(STYLE_PROPERTY_BACKGROUND_IMAGE, &image);
        return image;
    }

    DWORD StStyle::BackgroundRepeat()
    {
        DWORD repeat = 0;
        GetAttribute(STYLE_PROPERTY_BACKGROUND_REPEAT, &repeat, (DWORD)BACKGROUND_REPEAT_NULL);
        return repeat;
    }

    core::Color StStyle::TextColor()
    {
        core::Color color;
        GetAttribute(STYLE_PROPERTY_COLOR, &color);
        return color;
    }

    int StStyle::LineHeight()
    {
        int height = 0;
        GetAttribute(STYLE_PROPERTY_LINE_HEIGHT, &height, (int)16);
        return height;
    }

    DWORD StStyle::HorizontalAlignment()
    {
        DWORD halign = 0;
        GetAttribute(STYLE_PROPERTY_HORIZONTAL_ALIGN, &halign, (DWORD)ALIGNMENT_NULL);
        return halign;
    }

    DWORD StStyle::VerticalAlignment()
    {
        DWORD valign = 0;
        GetAttribute(STYLE_PROPERTY_VERTICAL_ALIGN, &valign, (DWORD)ALIGNMENT_NULL);
        return valign;
    }

    DWORD StStyle::TextAlignment()
    {
        DWORD valign = 0;
        GetAttribute(STYLE_PROPERTY_TEXT_ALIGN, &valign, (DWORD)ALIGNMENT_NULL);
        return valign;
    }

    bool StStyle::IsUnderline()
    {
        bool underline = false;
        GetAttribute(STYLE_PROPERTY_UNDERLINE, &underline, (bool)false);
        return underline;
    }

    int StStyle::TextIndent()
    {
        int indent = 0;
        GetAttribute(STYLE_PROPERTY_TEXT_INDENT, &indent, (int)0);
        return indent;
    }

    std::wstring StStyle::FontFamily()
    {
        std::wstring font_family;
        std::wstring default_val = DEFAULT_FONT_FAMILY;
        GetAttribute(STYLE_PROPERTY_FONT_FAMILY, &font_family, default_val);
        return font_family;
    }

    core::Unit StStyle::FontSize()
    {
        core::Unit size;
        core::Unit default_val(12);
        GetAttribute(STYLE_PROPERTY_FONT_SIZE, &size, default_val);
        return size;
    }

    DWORD StStyle::FontStyle()
    {
        DWORD style = 0;
        GetAttribute(STYLE_PROPERTY_FONT_STYLE, &style, (DWORD)FONT_STYLE_NORMAL);
        return style;
    }

    int StStyle::BorderWidth()
    {
        int border = 0;
        GetAttribute(STYLE_PROPERTY_BORDER_WIDTH, &border, (int)0);
        return border;
    }

    DWORD StStyle::BorderStyle()
    {
        DWORD style = 0;
        GetAttribute(STYLE_PROPERTY_BORDER_STYLE, &style, (DWORD)BORDER_STYLE_SOLID);
        return style;
    }

    core::Color StStyle::BorderColor()
    {
        core::Color color;
        GetAttribute(STYLE_PROPERTY_BORDER_COLOR, &color);
        return color;
    }

    DWORD StStyle::Position()
    {
        DWORD position = 0;
        GetAttribute(STYLE_PROPERTY_POSITION, &position, (DWORD)POSITION_RELATIVE);
        return position;
    }

    core::Unit StStyle::MarginLeft()
    {
        core::Unit unit;
        GetAttribute(STYLE_PROPERTY_MARGIN_LEFT, &unit);
        return unit;
    }

    core::Unit StStyle::MarginTop()
    {
        core::Unit unit;
        GetAttribute(STYLE_PROPERTY_MARGIN_TOP, &unit);
        return unit;
    }

    core::Unit StStyle::MarginRight()
    {
        core::Unit unit;
        GetAttribute(STYLE_PROPERTY_MARGIN_RIGHT, &unit);
        return unit;
    }

    core::Unit   StStyle::MarginBottom()
    {
        core::Unit unit;
        GetAttribute(STYLE_PROPERTY_MARGIN_BOTTOM, &unit);
        return unit;
    }

    core::Unit StStyle::PaddingLeft()
    {
        core::Unit unit;
        GetAttribute(STYLE_PROPERTY_PADDING_LEFT, &unit);
        return unit;
    }

    core::Unit StStyle::PaddingTop()
    {
        core::Unit unit;
        GetAttribute(STYLE_PROPERTY_PADDING_TOP, &unit);
        return unit;
    }

    core::Unit StStyle::PaddingRight()
    {
        core::Unit unit;
        GetAttribute(STYLE_PROPERTY_PADDING_RIGHT, &unit);
        return unit;
    }

    core::Unit StStyle::PaddingBottom()
    {
        core::Unit unit;
        GetAttribute(STYLE_PROPERTY_PADDING_BOTTOM, &unit);
        return unit;
    }

    core::Unit StStyle::Left()
    {
        core::Unit unit;
        GetAttribute(STYLE_PROPERTY_LEFT, &unit);
        return unit;
    }

    core::Unit StStyle::Top()
    {
        core::Unit unit;
        GetAttribute(STYLE_PROPERTY_TOP, &unit);
        return unit;
    }

    core::Unit StStyle::Right()
    {
        core::Unit unit;
        GetAttribute(STYLE_PROPERTY_RIGHT, &unit);
        return unit;
    }

    core::Unit StStyle::Bottom()
    {
        core::Unit unit;
        GetAttribute(STYLE_PROPERTY_BOTTOM, &unit);
        return unit;
    }

    core::Unit StStyle::Width()
    {
        core::Unit unit;
        GetAttribute(STYLE_PROPERTY_WIDTH, &unit, core::Unit(100, false));
        return unit;
    }

    core::Unit StStyle::Height()
    {
        core::Unit unit;
        GetAttribute(STYLE_PROPERTY_HEIGHT, &unit, core::Unit(100, false));
        return unit;
    }

    DWORD StStyle::CursorType()
    {
        DWORD cursor = 0;
        GetAttribute(STYLE_PROPERTY_CURSOR, &cursor, (DWORD)CURSOR_TYPE_ARROW);
        return cursor;
    }

    DWORD StStyle::BoxMode()
    {
        DWORD mode = 0;
        GetAttribute(STYLE_PROPERTY_BOX_MODE, &mode, (DWORD)BOX_MODE_CLASSIC);
        return mode;
    }

    void StStyle::SetBackgroundColor(const core::Color& color)
    {
        AddAttribute(STYLE_PROPERTY_BACKGROUND_COLOR, color);
    }

    void StStyle::SetBackgroundImage(const std::wstring& file)
    {
        AddAttribute(STYLE_PROPERTY_BACKGROUND_IMAGE, file);
    }

    void StStyle::SetBackgroundRepeat(DWORD repeat)
    {
        AddAttribute(STYLE_PROPERTY_BACKGROUND_REPEAT, repeat);
    }

    void StStyle::SetTextColor(const core::Color& color)
    {
        AddAttribute(STYLE_PROPERTY_COLOR, color);
    }

    void StStyle::SetLineHeight(int line_height)
    {
        AddAttribute(STYLE_PROPERTY_LINE_HEIGHT, line_height);
    }

    void StStyle::SetHorizontalAlignment(DWORD align)
    {
        AddAttribute(STYLE_PROPERTY_HORIZONTAL_ALIGN, align);
    }

    void StStyle::SetVerticalAlignment(DWORD align)
    {
        AddAttribute(STYLE_PROPERTY_VERTICAL_ALIGN, align);
    }

    void StStyle::SetTextAlignment(DWORD align)
    {
        AddAttribute(STYLE_PROPERTY_TEXT_ALIGN, align);
    }

    void StStyle::SetUnderline(bool underline)
    {
        AddAttribute(STYLE_PROPERTY_UNDERLINE, underline);
    }

    void StStyle::SetTextIndent(int indent)
    {
        AddAttribute(STYLE_PROPERTY_TEXT_INDENT, indent);
    }

    void StStyle::SetFontFamily(const std::wstring& family)
    {
        AddAttribute(STYLE_PROPERTY_FONT_FAMILY, family);
    }

    void StStyle::SetFontSize(const core::Unit& size)
    {
        AddAttribute(STYLE_PROPERTY_FONT_SIZE, size);
    }

    void StStyle::SetFontStyle(DWORD style)
    {
        AddAttribute(STYLE_PROPERTY_FONT_STYLE, style);
    }

    void StStyle::SetBorderWidth(int width)
    {
        AddAttribute(STYLE_PROPERTY_BORDER_WIDTH, width);
    }

    void StStyle::SetBorderStyle(DWORD style)
    {
        AddAttribute(STYLE_PROPERTY_BORDER_STYLE, style);
    }

    void StStyle::SetBorderColor(const core::Color& color)
    {
        AddAttribute(STYLE_PROPERTY_BORDER_COLOR, color);
    }

    void StStyle::SetPosition(DWORD position)
    {
        AddAttribute(STYLE_PROPERTY_POSITION, position);
    }

    void StStyle::SetMarginLeft(const core::Unit& unit)
    {
        AddAttribute(STYLE_PROPERTY_MARGIN_LEFT, unit);
    }

    void StStyle::SetMarginTop(const core::Unit& unit)
    {
        AddAttribute(STYLE_PROPERTY_MARGIN_TOP, unit);
    }

    void StStyle::SetMarginRight(const core::Unit& unit)
    {
        AddAttribute(STYLE_PROPERTY_MARGIN_RIGHT, unit);
    }

    void StStyle::SetMarginBottom(const core::Unit& unit)
    {
        AddAttribute(STYLE_PROPERTY_MARGIN_BOTTOM, unit);
    }

    void StStyle::SetPaddingLeft(const core::Unit& unit)
    {
        AddAttribute(STYLE_PROPERTY_PADDING_LEFT, unit);
    }

    void StStyle::SetPaddingTop(const core::Unit& unit)
    {
        AddAttribute(STYLE_PROPERTY_PADDING_TOP, unit);
    }

    void StStyle::SetPaddingRight(const core::Unit& unit)
    {
        AddAttribute(STYLE_PROPERTY_PADDING_RIGHT, unit);
    }

    void StStyle::SetPaddingBottom(const core::Unit& unit)
    {
        AddAttribute(STYLE_PROPERTY_PADDING_BOTTOM, unit);
    }

    void StStyle::SetLeft(const core::Unit& unit)
    {
        AddAttribute(STYLE_PROPERTY_LEFT, unit);
    }

    void StStyle::SetTop(const core::Unit& unit)
    {
        AddAttribute(STYLE_PROPERTY_TOP, unit);
    }

    void StStyle::SetRight(const core::Unit& unit)
    {
        AddAttribute(STYLE_PROPERTY_RIGHT, unit);
    }

    void StStyle::SetBottom(const core::Unit& unit)
    {
        AddAttribute(STYLE_PROPERTY_BOTTOM, unit);
    }

    void StStyle::SetWidth(const core::Unit& unit)
    {
        AddAttribute(STYLE_PROPERTY_WIDTH, unit);
    }

    void StStyle::SetHeight(const core::Unit& unit)
    {
        AddAttribute(STYLE_PROPERTY_HEIGHT, unit);
    }

    void StStyle::SetCursor(DWORD type)
    {
        AddAttribute(STYLE_PROPERTY_CURSOR, type);
    }

    void StStyle::SetBoxMode(DWORD mode)
    {
        AddAttribute(STYLE_PROPERTY_BOX_MODE, mode);
    }


    const char * StStyle::ParseValue(const char * p)
    {
        if (util::String::IsEmpty<char>(p))
        {
            return 0;
        }

        xml::XmlString name;
        p = xml::XmlBase::ReadText(p, &name, true, false, ":", true, xml::XML_DEFAULT_ENCODING);

        xml::XmlString value;
        if (strcmp(name.c_str(), STYLE_PROPERTY_BACKGROUND) == 0 ||
            strcmp(name.c_str(), STYLE_PROPERTY_FONT) == 0 ||
            strcmp(name.c_str(), STYLE_PROPERTY_BORDER) == 0 ||
            strcmp(name.c_str(), STYLE_PROPERTY_MARGIN) == 0 ||
            strcmp(name.c_str(), STYLE_PROPERTY_PADDING) == 0)
        {
            p = xml::XmlBase::ReadText(p, &value, false, false, ";", true, xml::XML_DEFAULT_ENCODING);
        }
        else
        {
            p = xml::XmlBase::ReadText(p, &value, true, false, ";", true, xml::XML_DEFAULT_ENCODING);
        }

        SetAttribute(name.c_str(), value.c_str(), 0);

        return p;
    }

    void StStyle::ParseValues(const char * p)
    {
        while (p && *p)
            p = ParseValue(p);
    }

    void StStyle::Copy(const StStyle& style)
    {
        for (MapType::const_iterator it = style.attributes_.begin(); it != style.attributes_.end(); ++it)
        {
            KeyAttribute key = it->first;
            MapType::iterator iter = attributes_.find(key);
            if (iter != attributes_.end())
            {
                iter->second->Drop();
                attributes_.erase(iter);
            }

            StObject* obj = it->second->Clone();
            if (!obj)
            {
                continue;
            }

            attributes_[key] = obj;
        }
    }

    void StStyle::Inherit(const StStyle& style)
    {
        for (MapType::const_iterator it = style.attributes_.begin(); it != style.attributes_.end(); ++it)
        {
            bool inherit = false;
            KeyAttribute key = it->first;
            if (KeyAttributeEquals(StringToKeyAttribute(STYLE_PROPERTY_LINE_HEIGHT), key) ||
                KeyAttributeEquals(StringToKeyAttribute(STYLE_PROPERTY_COLOR), key) ||
                KeyAttributeEquals(StringToKeyAttribute(STYLE_PROPERTY_FONT_FAMILY), key) ||
                KeyAttributeEquals(StringToKeyAttribute(STYLE_PROPERTY_FONT_SIZE), key) ||
                KeyAttributeEquals(StringToKeyAttribute(STYLE_PROPERTY_FONT_STYLE), key) ||
                KeyAttributeEquals(StringToKeyAttribute(STYLE_PROPERTY_TEXT_ALIGN), key))
            {
                inherit = true;
            }

            if (inherit)
            {
                MapType::iterator iter = attributes_.find(key);
                if (iter != attributes_.end())
                {
                    iter->second->Drop();
                    attributes_.erase(iter);
                }

                StObject* obj = it->second->Clone();
                if (!obj)
                {
                    continue;
                }

                attributes_[key] = obj;
            }
        }
    }

    


    bool StStyle::ParseBackground(const char *p)
    {
        if (util::String::IsEmpty<char>(p))
        {
            return false;
        }

        util::TString<char> tstring(p);
        tstring.Trim();

        std::string color;
        std::string::size_type pos = tstring.Tokenize(" ", 0, color);
        SetAttribute(STYLE_PROPERTY_BACKGROUND_COLOR, color.c_str(), 0);
        if (pos == std::string::npos)
        {
            return true;
        }

        std::string file;
        pos = tstring.Tokenize(" ", pos, file);
        SetAttribute(STYLE_PROPERTY_BACKGROUND_IMAGE, file.c_str(), 0);
        if (pos == std::string::npos)
        {
            return true;
        }

        std::string repeat;
        pos = tstring.Tokenize(" ", pos, repeat);
        SetAttribute(STYLE_PROPERTY_BACKGROUND_REPEAT, repeat.c_str(), 0);

        return true;
    }

    bool StStyle::ParseMargin(const char *p)
    {
        if (util::String::IsEmpty<char>(p))
        {
            return false;
        }

        util::TString<char> tstring(p);
        tstring.Trim();

        std::string left;
        std::string::size_type pos = tstring.Tokenize(" ", 0, left);
        SetAttribute(STYLE_PROPERTY_MARGIN_LEFT, left.c_str(), 0);

        if (pos == std::string::npos)
        {
            SetAttribute(STYLE_PROPERTY_MARGIN_TOP, left.c_str(), 0);
            SetAttribute(STYLE_PROPERTY_MARGIN_RIGHT, left.c_str(), 0);
            SetAttribute(STYLE_PROPERTY_MARGIN_BOTTOM, left.c_str(), 0);
            return true;
        }

        std::string top;
        pos = tstring.Tokenize(" ", pos, top);
        SetAttribute(STYLE_PROPERTY_MARGIN_TOP, top.c_str(), 0);
        if (pos == std::string::npos)
        {
            return true;
        }

        std::string right;
        pos = tstring.Tokenize(" ", pos, right);
        SetAttribute(STYLE_PROPERTY_MARGIN_RIGHT, right.c_str(), 0);
        if (pos == std::string::npos)
        {
            return true;
        }

        std::string bottom;
        pos = tstring.Tokenize(" ", pos, bottom);
        SetAttribute(STYLE_PROPERTY_MARGIN_BOTTOM, bottom.c_str(), 0);

        return true;
    }

    bool StStyle::ParsePadding(const char *p)
    {
        if (util::String::IsEmpty<char>(p))
        {
            return false;
        }

        util::TString<char> tstring(p);
        tstring.Trim();

        std::string left;
        std::string::size_type pos = tstring.Tokenize(" ", 0, left);
        SetAttribute(STYLE_PROPERTY_PADDING_LEFT, left.c_str(), 0);

        if (pos == std::string::npos)
        {
            SetAttribute(STYLE_PROPERTY_PADDING_TOP, left.c_str(), 0);
            SetAttribute(STYLE_PROPERTY_PADDING_RIGHT, left.c_str(), 0);
            SetAttribute(STYLE_PROPERTY_PADDING_BOTTOM, left.c_str(), 0);
            return true;
        }

        std::string top;
        pos = tstring.Tokenize(" ", pos, top);
        SetAttribute(STYLE_PROPERTY_PADDING_TOP, top.c_str(), 0);
        if (pos == std::string::npos)
        {
            return true;
        }

        std::string right;
        pos = tstring.Tokenize(" ", pos, right);
        SetAttribute(STYLE_PROPERTY_PADDING_RIGHT, right.c_str(), 0);
        if (pos == std::string::npos)
        {
            return true;
        }

        std::string bottom;
        pos = tstring.Tokenize(" ", pos, bottom);
        SetAttribute(STYLE_PROPERTY_PADDING_BOTTOM, bottom.c_str(), 0);

        return true;
    }

    bool StStyle::ParseFont(const char *p)
    {
        if (util::String::IsEmpty<char>(p))
        {
            return false;
        }

        util::TString<char> tstring(p);
        tstring.Trim();

        std::string family;
        std::string::size_type pos = tstring.Tokenize(" ", 0, family);
        SetAttribute(STYLE_PROPERTY_FONT_FAMILY, family.c_str(), 0);
        if (pos == std::string::npos)
        {
            return true;
        }

        std::string size;
        pos = tstring.Tokenize(" ", pos, size);
        SetAttribute(STYLE_PROPERTY_FONT_SIZE, size.c_str(), 0);
        if (pos == std::string::npos)
        {
            return true;
        }

        std::string style;
        pos = tstring.Tokenize(" ", pos, style);
        SetAttribute(STYLE_PROPERTY_FONT_STYLE, style.c_str(), 0);

        return true;
    }

    bool StStyle::ParseBorder(const char *p)
    {
        if (util::String::IsEmpty<char>(p))
        {
            return false;
        }

        util::TString<char> tstring(p);
        tstring.Trim();

        std::string width;
        std::string::size_type pos = tstring.Tokenize(" ", 0, width);
        SetAttribute(STYLE_PROPERTY_BORDER_WIDTH, width.c_str(), 0);
        if (pos == std::string::npos)
        {
            return true;
        }

        std::string style;
        pos = tstring.Tokenize(" ", pos, style);
        SetAttribute(STYLE_PROPERTY_BORDER_STYLE, style.c_str(), 0);
        if (pos == std::string::npos)
        {
            return true;
        }

        std::string color;
        pos = tstring.Tokenize(" ", pos, color);
        SetAttribute(STYLE_PROPERTY_BORDER_COLOR, color.c_str(), 0);

        return true;
    }

    bool StStyle::ParserBorderRadius( const char *p )
    {
        if (util::String::IsEmpty<char>(p))
        {
            return false;
        }

        util::TString<char> tstring(p);
        tstring.Trim();

        std::string width;
        std::string::size_type pos = tstring.Tokenize("/", 0, width);
        SetAttribute(STYLE_PROPERTY_BORDER_RADIUS_WIDTH, width.c_str(), 0);
        if (pos == std::string::npos)
        {
            SetAttribute(STYLE_PROPERTY_BORDER_RADIUS_HEIGHT, width.c_str(), 0);
            return true;
        }

        std::string height;
        pos = tstring.Tokenize("/", pos, height);
        SetAttribute(STYLE_PROPERTY_BORDER_RADIUS_HEIGHT, height.c_str(), 0);

        return true;
    }

    int StStyle::BorderRadiusWidth()
    {
        int radius = 0;
        GetAttribute(STYLE_PROPERTY_BORDER_RADIUS_WIDTH, &radius, (int)0);
        return radius;
    }

    int StStyle::BorderRadiusHeight()
    {
        int radius = 0;
        GetAttribute(STYLE_PROPERTY_BORDER_RADIUS_HEIGHT, &radius, (int)0);
        return radius;
    }

    void StStyle::SetBorderRadiusWidth( int nRadiusWidth)
    {
        AddAttribute(STYLE_PROPERTY_BORDER_RADIUS_WIDTH, nRadiusWidth);
    }

    void StStyle::SetBorderRadiusHeight( int nRadiusHeight)
    {
        AddAttribute(STYLE_PROPERTY_BORDER_RADIUS_HEIGHT, nRadiusHeight);
    }
}