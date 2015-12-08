#ifndef __animate_linear_animate_h__
#define __animate_linear_animate_h__

#include <windows.h>
#include <tchar.h>

#include <string>
#include <vector>

#include "animate/def.h"
#include "animate/tween.h"
#include "animate/attribute_animate.h"


namespace animate
{
    class AttributeLinerAnimate : public AttributeAnimate
    {
    public:
        AttributeLinerAnimate();

        AttributeLinerAnimate(
            std::string id,
            std::string attr_name,
            int begin_value,
            int end_value,
            LinerAnimateStyle style);

        void SetBeginValue(int begin_value);
        int BeginValue();

        void SetEndValue(int end_value);
        int EndValue();

        virtual void DoStep();

    private:
        int         begin_value_;
        int         end_value_;
        LinerAnimateStyle liner_style_;
    };
}

#endif