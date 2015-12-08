#ifndef __animate_def_h__
#define __animate_def_h__

namespace animate
{
    enum AnimateType
    {
        AnimateTypeNull,
        AnimateTypeLinear,
        AnimateTypeFrame,
    };

    enum LinerAnimateStyle
    {
        LinerAnimateStyleNull,
        LinerAnimateStyleLiner,
    };

    static const int DEFAULT_TIMER_INTERVAL = 10U;
}

#endif