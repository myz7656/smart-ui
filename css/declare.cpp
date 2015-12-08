#include "css/declare.h"

namespace css
{
    Declare::Declare() {}

    Declare::~Declare() {}

    bool Declare::HasValue()
    {
        return !property_.empty() &&
               !value_.empty();
    }

    std::wstring Declare::Property()
    {
        return property_;
    }

    std::wstring Declare::Value()
    {
        return value_;
    }

    const wchar_t* Declare::Parse(const wchar_t* text)
    {
        bool value = false;
        const wchar_t* p = text;
        while (p && *p)
        {
            if (*p == L':')
            {
                ++p;
                value = true;
                continue;
            }
            else if (*p == L';')
            {
                break;
            }

            if (value)
            {
                value_ += *p;
            }
            else
            {
                property_ += *p;
            }
            ++p;
        }
        return p;
    }



    DeclareSet::DeclareSet() {}

    DeclareSet::~DeclareSet() {}

    bool DeclareSet::HasDeclares()
    {
        return declares_.size() > 0;
    }

    const wchar_t* DeclareSet::Parse(const wchar_t* text)
    {
        const wchar_t* p = text;
        while (p && *p)
        {
            if (iswspace(*p))
            {
                ++p;
                continue;
            }

            if (*p == L'}')
            {
                break;
            }

            Declare declare;
            p = declare.Parse(p);
            if (!p || !*p || *p != L';')
            {
                break;
            }
            ++p;

            if (declare.HasValue())
            {
                declares_.push_back(declare);
            }
        }
        return p;
    }

    std::vector<Declare>& DeclareSet::Declares()
    {
        return declares_;
    }

    void DeclareSet::Clear()
    {
        declares_.clear();
    }

    void DeclareSet::Push(DeclareSet& declare)
    {
        declares_.insert(declares_.end(), declare.Declares().begin(), declare.Declares().end());
    }
}