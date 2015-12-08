#ifndef __animate_frame_animate_h__
#define __animate_frame_animate_h__

#include "animate/attribute_animate.h"

namespace animate
{
    class AttributeFrameAnimate : public AttributeAnimate
    {
    public:
        AttributeFrameAnimate();
        AttributeFrameAnimate(int count);
        AttributeFrameAnimate(std::string id, std::string attr_name, int count);

        void SetCount(int count);
        int Count();

        virtual void DoStep();

    private:
        int count_;
        int current_count_;
    };
}

#endif