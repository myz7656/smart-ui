#ifndef __css_declare_h__
#define __css_declare_h__

#include <vector>
#include <string>

namespace css
{
    class Declare
    {
    public:
        Declare();
        ~Declare();

        bool HasValue();

        std::wstring Property();
        std::wstring Value();

        const wchar_t* Parse(const wchar_t* text);

    private:
        std::wstring property_;
        std::wstring value_;
    };



    class DeclareSet
    {
    public:
        DeclareSet();
        ~DeclareSet();

        bool HasDeclares();

        const wchar_t* Parse(const wchar_t* text);

        std::vector<Declare>& Declares();

        void Clear();

        void Push(DeclareSet& declare);

    private:
        std::vector<Declare> declares_;
    };
}

#endif