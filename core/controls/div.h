#ifndef __core_controls_div_h__
#define __core_controls_div_h__

#include "core/def.h"
#include "core/container.h"

#include <string>

namespace core
{
    class StDivision : public StContainer
    {
    public:
        DECLARE_DYNAMIC_CREATE(StDivision);

        StDivision() {}
        ~StDivision() {}

    private:
        DISABLE_COPY_AND_ASSIGN(StDivision)
    };
};
#endif