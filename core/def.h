#ifndef __core_def_h__
#define __core_def_h__

#include <windows.h>

namespace core
{
    class StObject;
}

typedef core::StObject* (__stdcall *DYNAMIC_CREATE_FN)(const char*);
struct RuntimeClass
{
    static RuntimeClass* first_class_;
    static core::StObject* CreateObject(const char*);

    DYNAMIC_CREATE_FN pfn_create_object_;
    RuntimeClass* next_class_;
};

struct CLASS_INIT
{
    CLASS_INIT(RuntimeClass* new_class)
    {
        new_class->next_class_ = RuntimeClass::first_class_;
        RuntimeClass::first_class_ = new_class;
    }
};

#define DECLARE_DYNAMIC_CREATE(class_name)\
public:\
    static RuntimeClass class##class_name;\
    static core::StObject*__stdcall CreateObject(const char*);\
    static bool EqualTag(const std::string& tag);

#define IMPLY_DYNAMIC_CREATE(class_name, tag_name)\
    RuntimeClass class_name::class##class_name =\
    {\
        class_name::CreateObject,\
        0\
    };\
    static CLASS_INIT _init_##class_name(&class_name::class##class_name);\
    core::StObject* class_name::CreateObject(const char* value)\
    {\
        if (strcmp(value, tag_name) == 0)\
            return new class_name;\
        \
        return 0;\
    }\
    bool class_name::EqualTag(const std::string& tag)\
    {\
        if (strcmp(tag.c_str(), tag_name) == 0) \
            return true;\
        \
        return false;\
    }


#endif