#ifndef __core_attribute_h__
#define __core_attribute_h__

#include <map>
#include <string>

#include "base/def.h"

#include "core/object.h"
#include "core/types.h"

#include "util/string.h"

#include "xml/xml.h"

namespace core
{
    #define DECLARE_ATTRIBUTE_MAP()\
        virtual bool SetAttribute(const char *name, const char *value, DWORD* result);

    #define BEGIN_ATTRIBUTE_MAP(obj)\
        bool obj::SetAttribute(\
            const char *name,\
            const char *value,\
            DWORD* result)\
        {\
            if (result) \
                *result = 0;\
            if (!name || !*name || !value || !*value)\
            {\
                return false;\
            }\
            bool ret;

    #define END_ATTRIBUTE_MAP()\
                ret = false;\
            \
            return ret;\
        }

    #define CHAIN_ATTRIBUTE_MAP(theclass)\
            if (theclass::SetAttribute(name, value, result))\
            {\
                ret = true;\
            }\
            else

    #define CHAIN_MEMBER_ATTRIBUTE_MAP(themember)\
            if (themember.SetAttribute(name, value, result))\
            {\
                ret = true;\
            }\
            else
 
    #define ATTRIBUTE(attribname, classtype, state)\
            if (strcmp(name, attribname) == 0)\
            {\
                classtype::ValueType v = classtype::Parse(value);\
                AddAttribute(name, v);\
                ret = true;\
                if (result)\
                    *result = state;\
            }\
            else

    #define ATTRIBUTE_PARSER(attribname, parser, state)\
            if (strcmp(name, attribname) == 0)\
            {\
                parser(value);\
                ret = true;\
                if (result)\
                    *result = state;\
            }\
            else

    #define ATTRIBUTE_ENUM_BEGIN(attribname, classtype)\
            if (strcmp(name, attribname) == 0)\
            {\
                classtype::ValueType varTemp;

    #define ATTRIBUTE_ENUM_VALUE(enumstring, enumvalue)\
                if (strcmp(value, enumstring) == 0)\
                {\
                    varTemp = enumvalue;\
                    ret = true;\
                }\
                else

    #define ATTRIBUTE_ENUM_END(state)\
                    ret = false;\
                if (ret)\
                {\
                    AddAttribute(name, varTemp);\
                    if (result)\
                        *result = state;\
                }\
            }\
            else



    template<typename Type>
    class StAttribute : public StObject
    {
    public:
        typedef Type ValueType;

        StAttribute()
            : value_(Type()) {}

        ~StAttribute() {}

        void Set(const Type& value)
        {
            value_ = value;
        }

        Type Get()
        {
            return value_;
        }

        StObject* Clone()
        {
            StAttribute* attribute = new StAttribute;
            if (!attribute)
            {
                return 0;
            }

            attribute->value_ = value_;
            return static_cast<StObject*>(attribute);
        }

        void Drop()
        {
            delete this;
        }

    private:
        Type value_;

    private:
        DISABLE_COPY_AND_ASSIGN(StAttribute)
    };

    class StAttributeBool : public StAttribute<bool>
    {
    public:
        typedef StAttribute<bool>::ValueType ValueType;

        static bool Parse(const char* value)
        {
            if (util::String::IsEmpty<char>(value))
            {
                return false;
            }
            int val = ::atoi(value);
            return val != 0;
        }
    };

    class StAttributeInteger : public StAttribute<int>
    {
    public:
        typedef StAttribute<int>::ValueType ValueType;

        static int Parse(const char* value)
        {
            if (util::String::IsEmpty<char>(value))
            {
                return 0;
            }
            int val = ::atoi(value);
            return val;
        }
    };

    class StAttributeLong : public StAttribute<long>
    {
    public:
        typedef StAttribute<long>::ValueType ValueType;

        static long Parse(const char* value)
        {
            if (util::String::IsEmpty<char>(value))
            {
                return 0l;
            }
            long val = ::atol(value);
            return val;
        }
    };

    class StAttributeDword : public StAttribute<DWORD>
    {
    public:
        typedef StAttribute<DWORD>::ValueType ValueType;

        static DWORD Parse(const char* value)
        {
            if (util::String::IsEmpty<char>(value))
            {
                return 0;
            }
            DWORD val = ::strtoul(value, 0, 10);
            return val;
        }
    };

    class StAttributeString : public StAttribute<std::string>
    {
    public:
        typedef StAttribute<std::string>::ValueType ValueType;

        static std::string Parse(const char* value)
        {
            if (util::String::IsEmpty<char>(value))
            {
                return "";
            }
            std::string val = value;
            return val;
        }
    };

    class StAttributeWString : public StAttribute<std::wstring>
    {
    public:
        typedef StAttribute<std::wstring>::ValueType ValueType;

        static std::wstring Parse(const char* value)
        {
            if (util::String::IsEmpty<char>(value))
            {
                return L"";
            }
            std::wstring val = util::String::UTF8ToUnicode(value);
            return val;
        }
    };

    class StAttributeColor : public StAttribute<core::Color>
    {
    public:
        typedef StAttribute<core::Color>::ValueType ValueType;

        static core::Color Parse(const char* value)
        {
            core::Color color;
            if (util::String::IsEmpty<char>(value))
            {
                return color;
            }

            const char *q = value;
            if (*q == '#')
            {
                q++;
                if (strlen(q) == 6)
                {
                    unsigned char r = util::String::HexToT<unsigned char>(q,     2);
                    unsigned char g = util::String::HexToT<unsigned char>(q + 2, 2);
                    unsigned char b = util::String::HexToT<unsigned char>(q + 4, 2);
                    color = (core::Color)( r | ((g << 8) & 0xff00) | ((b << 16) & 0xff0000));
                }
            }
            else
            {
                xml::XmlString str;
                if ((q = xml::XmlBase::ReadText(q, &str, true, false, "(", true, xml::XML_DEFAULT_ENCODING)) &&
                    _strnicmp(str.c_str(), "rgb", 3) == 0)
                {
                    xml::XmlString sRed, sGreen, sBlue;
                    q = xml::XmlBase::ReadText(q, &sRed, true, false, ",", true, xml::XML_DEFAULT_ENCODING);
                    if (sRed.size() == 0)
                        return color;

                    q = xml::XmlBase::ReadText( q, &sGreen, true, false, ",", true, xml::XML_DEFAULT_ENCODING );
                    if (sGreen.size() == 0)
                        return color;

                    q = xml::XmlBase::ReadText(q, &sBlue, true, false, ")", true, xml::XML_DEFAULT_ENCODING);
                    if (sBlue.size() == 0)
                        return color;

                    unsigned char r = (unsigned char)::atoi(sRed.c_str());
                    unsigned char g = (unsigned char)::atoi(sGreen.c_str());
                    unsigned char b = (unsigned char)::atoi(sBlue.c_str());

                    color = (core::Color)(r | ((g << 8) & 0xff00) | ((b << 16) & 0xff0000));
                }
            }

            return color;
        }
    };

    class StAttributeUnit : public StAttribute<core::Unit>
    {
    public:
        typedef StAttribute<core::Unit>::ValueType ValueType;

        static core::Unit Parse(const char* value)
        {
            core::Unit unit;
            if (util::String::IsEmpty<char>(value))
            {
                return unit;
            }

            const char *p = value;
            xml::XmlString str;
            if (strstr(p, "%") != 0)
            {
                p = xml::XmlBase::ReadText(p, &str, true, false, "%", true, xml::XML_DEFAULT_ENCODING);
                if (str.size() != 0)
                {
                    unit.use = ::atoi(str.c_str());
                    unit.pixel = false;
                }
            }
            else if (strstr(p, "px" ) != 0)
            {
                p = xml::XmlBase::ReadText(p, &str, true, false, "px", true, xml::XML_DEFAULT_ENCODING);
                if (str.size() != 0)
                {
                    unit.use = ::atoi(str.c_str());
                    unit.pixel = true;
                }
            }
            else
            {
                unit.use = ::atoi(p);
                unit.pixel = true;
            }

            return unit;
        }
    };

    class StAttributePixel : public StAttribute<int>
    {
    public:
        typedef StAttribute<int>::ValueType ValueType;

        static int Parse(const char *value)
        {
            if (util::String::IsEmpty<char>(value))
            {
                return 0;
            }

            const char* p = value;
            int pixel = 0;
            xml::XmlString str;
            if (strstr(p, "px") != 0)
            {
                p = xml::XmlBase::ReadText(p, &str, true, false, "px", true, xml::XML_DEFAULT_ENCODING);
                if (str.size() != 0)
                {
                    pixel = ::atoi(str.c_str());
                }
            }
            else
            {
                pixel = ::atoi(p);
            }

            return pixel;
        }
    };

    class StAttributeFile : public StAttribute<std::wstring>
    {
    public:
        typedef StAttribute<std::wstring>::ValueType ValueType;

        static std::wstring Parse(const char* value)
        {
            if (util::String::IsEmpty<char>(value))
            {
                return L"";
            }

            const char* p = value;
            std::wstring file;

            xml::XmlString str;
            p = xml::XmlBase::ReadText(p, &str, true, false, "file(", true, xml::XML_DEFAULT_ENCODING);
            if (p && *p)
            {
                p = xml::XmlBase::ReadText(p, &str, true, false, ")", true, xml::XML_DEFAULT_ENCODING);
                if (str.size() != 0)
                {
                    file = util::String::UTF8ToUnicode(str.c_str());
                }
            }
            return file;
        }
    };




    class StAttributeSet : public StObject
    {
    public:
        typedef std::map<KeyAttribute, StObject*> MapType;

        StAttributeSet() {}

        StAttributeSet(const StAttributeSet& set)
        {
            Copy(set);
        }

        StAttributeSet& operator=(const StAttributeSet& set)
        {
            if (this == &set)
            {
                return *this;
            }

            Copy(set);
            return *this;
        }

        virtual ~StAttributeSet()
        {
            Clear();
        }

        StObject* Clone()
        {
            StAttributeSet* attrs = new StAttributeSet(*this);
            if (!attrs)
            {
                return 0;
            }
            return static_cast<StObject*>(attrs);
        }

        void Drop()
        {
            delete this;
        }

        template<typename Type>
        bool AddAttribute(const std::string& attribute, const Type& value)
        {
            if (attribute.empty())
            {
                return false;
            }

            KeyAttribute key = StringToKeyAttribute(attribute);
            MapType::iterator it = attributes_.find(key);
            if (it != attributes_.end())
            {
                StAttribute<Type>* attribute = static_cast<StAttribute<Type>*>(it->second);
                attribute->Set(value);
                return true;
            }

            StAttribute<Type>* obj = new StAttribute<Type>;
            if (obj)
            {
                obj->Set(value);
                attributes_[key] = obj;
                return true;
            }

            return false;
        }

        template<typename Type>
        bool GetAttribute(const std::string& attribute, Type* value, Type default_value = Type())
        {
            if (attribute.empty() || !value)
            {
                return false;
            }
            *value = default_value;

            KeyAttribute key = StringToKeyAttribute(attribute);
            MapType::iterator it = attributes_.find(key);
            if (it != attributes_.end())
            {
                StAttribute<Type>* attribute = static_cast<StAttribute<Type>*>(it->second);
                *value = attribute->Get();
                return true;
            }

            return false;
        }

        bool HasAttribute(const std::string& attribute)
        {
            if (attribute.empty())
            {
                return false;
            }

            KeyAttribute key = StringToKeyAttribute(attribute);
            MapType::iterator it = attributes_.find(key);
            if (it != attributes_.end())
            {
                return true;
            }
            return false;
        }

        void Clear()
        {
            MapType::iterator it = attributes_.begin();
            while (it != attributes_.end())
            {
                it->second->Drop();
                 ++it;
            }

            attributes_.clear();
        }

    protected:
        void Copy(const StAttributeSet& set)
        {
            Clear();

            for (MapType::const_iterator it = set.attributes_.begin(); it != set.attributes_.end(); ++it)
            {
                StObject* obj = it->second->Clone();
                if (!obj)
                {
                    continue;
                }

                attributes_[it->first] = obj;
            }
        }

        void CopyTo(StAttributeSet* set)
        {
            set->Clear();

            for (MapType::iterator it = attributes_.begin(); it != attributes_.end(); ++it)
            {
                StObject* obj = it->second->Clone();
                if (!obj)
                {
                    continue;
                }

                set->attributes_[it->first] = obj;
            }
        }

    protected:
        MapType attributes_;
    };
}

#endif