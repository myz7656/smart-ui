#include "animate/frame_animate.h"

namespace animate
{
    AttributeFrameAnimate::AttributeFrameAnimate()
        : AttributeAnimate(AnimateTypeFrame)
        , count_(0)
        , current_count_(0) {}

    AttributeFrameAnimate::AttributeFrameAnimate(int count)
        : AttributeAnimate(AnimateTypeFrame)
        , count_(count)
        , current_count_(0) {}

    AttributeFrameAnimate::AttributeFrameAnimate(std::string id, std::string attr_name, int count)
        : AttributeAnimate(id, attr_name, AnimateTypeFrame)
        , count_(count)
        , current_count_(0) {}

    void AttributeFrameAnimate::SetCount(int count)
    {
        count_ = count;
    }

    int AttributeFrameAnimate::Count()
    {
        return count_;
    }

    void AttributeFrameAnimate::DoStep()
    {
        int count = Count();
        if (count <= 0)
        {
            count = 1;
        }

        current_count_ = current_count_ % count;
        char str_value[16] = {0};
        _itoa_s(current_count_, str_value, 10);
        current_count_++;
        if (GetDelegate())
        {
            GetDelegate()->OnAttributeValueChanged(Id(), Attribute(), std::string(str_value));
        }
    }
}