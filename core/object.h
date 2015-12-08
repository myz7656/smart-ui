#ifndef __core_object_h__
#define __core_object_h__

#include <string>

namespace core
{
    typedef std::string KeyAttribute;
    typedef std::string KeyWidget;

    KeyAttribute StringToKeyAttribute(const std::string& id);
    KeyWidget    StringToKeyWidget   (const std::string& id);

    inline bool KeyAttributeEquals(KeyAttribute src, KeyAttribute dest)
    {
        return strcmp(src.c_str(), dest.c_str()) == 0;
    }

    inline bool KeyWidgetEquals(KeyWidget src, KeyWidget dest)
    {
        return strcmp(src.c_str(), dest.c_str()) == 0;
    }

    class StObject
    {
    public:
        virtual ~StObject() = 0;
        virtual StObject* Clone() = 0;
        virtual void Drop() = 0;
    };
}

#endif