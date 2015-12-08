#ifndef __core_const_h__
#define __core_const_h__

namespace core
{
    enum
    {
        ATTRIBUTE_TRAIT_NULL        = 0x00000000,
        ATTRIBUTE_TRAIT_LAYOUT      = 0x00000001,
        ATTRIBUTE_TRAIT_PAINT       = 0x00000002,
        ATTRIBUTE_TRAIT_BOTH        = ATTRIBUTE_TRAIT_LAYOUT | ATTRIBUTE_TRAIT_PAINT
    };

    enum
    {
        BACKGROUND_REPEAT_NULL    = 0x00000000,
        BACKGROUND_REPEAT_X       = 0x00000001,
        BACKGROUND_REPEAT_Y       = 0x00000002
    };

    enum
    {
        POSITION_ABSOLUTE        = 0x00000001,
        POSITION_RELATIVE        = 0x00000002
    };

    enum
    {
        ALIGNMENT_NULL                = 0x00000000,

        HORIZONTAL_ALIGNMENT_LEFT     = 0x00000001,
        HORIZONTAL_ALIGNMENT_CENTER   = 0x00000002,
        HORIZONTAL_ALIGNMENT_RIGHT    = 0x00000004,

        VERTICAL_ALIGNMENT_TOP        = 0x00000010,
        VERTICAL_ALIGNMENT_MIDDLE     = 0x00000020,
        VERTICAL_ALIGNMENT_BOTTOM     = 0x00000040
    };

    enum
    {
        FONT_STYLE_NORMAL         = 0x00000000,
        FONT_STYLE_BOLD           = 0x00000001,
        FONT_STYLE_ITALIC         = 0x00000002,
        FONT_STYLE_BOLD_ITALIC    = 0x00000003,
        FONT_STYLE_UNDERLINE      = 0x00000004
    };

    enum
    {
        BORDER_STYLE_SOLID        = 0x00000001,
        BORDER_STYLE_DASHED       = 0x00000002,
        BORDER_STYLE_DOTTED       = 0x00000004
    };

    enum
    {
        POSITION_TYPE_LEFT_TOP         = 0x00000001,
        POSITION_TYPE_LEFT_BOTTOM      = 0x00000002,
        POSITION_TYPE_RIGHT_TOP        = 0x00000004,
        POSITION_TYPE_RIGHT_BOTTOM     = 0x00000008
    };

    enum
    {
        CURSOR_TYPE_ARROW     = 0x00000001,
        CURSOR_TYPE_HAND      = 0x00000002
    };

    enum
    {
        BOX_MODE_CLASSIC     = 0x00000001,
        BOX_MODE_MODERN      = 0x00000002
    };

    enum
    {
        ACTION_STATE_NORMAL     = 0x00000001,
        ACTION_STATE_HOVER      = 0x00000002,
        ACTION_STATE_DOWN       = 0x00000004
    };

    enum
    {
        SCROLL_BAR_HORIZONTAL   = 0x00000001,
        SCROLL_BAR_VERTICAL     = 0x00000002
    };
}


#endif