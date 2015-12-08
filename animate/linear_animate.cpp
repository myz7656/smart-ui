#include "animate/linear_animate.h"

namespace animate
{
    AttributeLinerAnimate::AttributeLinerAnimate()
        : AttributeAnimate(AnimateTypeLinear)
        , begin_value_(0)
        , end_value_(0)
        , liner_style_(LinerAnimateStyleNull) {}

    AttributeLinerAnimate::AttributeLinerAnimate(
                std::string id,
                std::string attr_name,
                int begin_value,
                int end_value,
                LinerAnimateStyle style)
        : AttributeAnimate(id, attr_name, AnimateTypeLinear)
        , begin_value_(begin_value)
        , end_value_(end_value)
        , liner_style_(style) {}

    void AttributeLinerAnimate::SetBeginValue(int begin_value)
    {
        begin_value_ = begin_value;
    }

    int AttributeLinerAnimate::BeginValue()
    {
        return begin_value_;
    }

    void AttributeLinerAnimate::SetEndValue(int end_value)
    {
        end_value_ = end_value;
    }

    int AttributeLinerAnimate::EndValue()
    {
        return end_value_;
    }

    void AttributeLinerAnimate::DoStep()
    {
        float current = (float)Current();
        float total = (float)Total();

        int value = 0;
        switch(liner_style_)
        {
        case LinerAnimateStyleLiner:
            value = tween::Linear<int>::calculate(
                current,
                begin_value_,
                end_value_ - begin_value_,
                total);
            break;

        default:
            break;
        }

        char str_value[16] = {0};
        _itoa_s(value, str_value, 10);

        if (GetDelegate())
        {
            GetDelegate()->OnAttributeValueChanged(Id(), Attribute(), std::string(str_value));
        }
    }
}