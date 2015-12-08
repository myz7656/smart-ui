#ifndef __animate_attribute_animate_h__
#define __animate_attribute_animate_h__

#include "animate/def.h"
#include "animate/base.h"

namespace animate
{
    class AttributeAnimate : public Animate
    {
    public:
        class Delegate
        {
        public:
            virtual void OnAttributeValueChanged(std::string id, std::string attribute, std::string value) = 0;
        };

        AttributeAnimate()
            : delegate_(0)
            , type_(AnimateTypeNull) {}

        AttributeAnimate(AnimateType type)
            : delegate_(0)
            , type_(type) {}

        AttributeAnimate(std::string id, std::string attr_name, AnimateType type = AnimateTypeNull)
            : delegate_(0)
            , id_(id)
            , attribute_(attr_name)
            , type_(type) {}

        virtual ~AttributeAnimate() {}

        void SetDelegate(Delegate* delegate)     { delegate_ = delegate; }
        void SetId(std::string id)               { id_ = id; }
        void SetAttribute(std::string attribute) { attribute_ = attribute; }
        void SetType(AnimateType type)           { type_ = type; }

        Delegate* GetDelegate() { return delegate_; }
        std::string Id()        { return id_; }
        std::string Attribute() { return attribute_; }
        AnimateType Type()      { return type_; }

    private:
        Delegate*   delegate_;
        std::string id_;
        std::string attribute_;
        AnimateType type_;
    };
}

#endif