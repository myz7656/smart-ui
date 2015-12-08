#include "core/def.h"
#include "core/object.h"

RuntimeClass* RuntimeClass::first_class_ = 0;

core::StObject* RuntimeClass::CreateObject(const char* id)
{
    if (!id || !*id)
    {
        return 0;
    }

    core::StObject* obj = 0;
    for (RuntimeClass* next = RuntimeClass::first_class_; next != 0; next = next->next_class_)
    {
        if (!next->pfn_create_object_) 
        {
            continue;
        }

        obj = next->pfn_create_object_(id);
        if (obj)
        {
            break;
        }
    }

    return obj;
}

namespace core
{
    StObject::~StObject() {}

    KeyAttribute StringToKeyAttribute(const std::string& id)
    {
        return id;
    }

    KeyWidget StringToKeyWidget(const std::string& id)
    {
        return id;
    }
}