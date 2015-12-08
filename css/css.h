#ifndef __css_css_h__
#define __css_css_h__

#include "css/selector.h"
#include "css/declare.h"

namespace css
{
    class CssDocument
    {
    public:
        struct Pair
        {
            SelectorSet selectors;
            DeclareSet declares;
        };

        CssDocument();
        ~CssDocument();

        bool Parse(const char* text);
        void Clear();

        bool FindSelectorByTag(
            const std::wstring& tag_name,
            const std::wstring& pseudo_class,
            DeclareSet& declares);

        bool FindSelectorByClass(
            const std::wstring& class_name,
            const std::wstring& tag_name,
            const std::wstring& pseudo_class,
            DeclareSet& declares);

        bool FindSelectorById(
            const std::wstring& id,
            const std::wstring& tag_name,
            const std::wstring& pseudo_class,
            DeclareSet& declares);

    private:
        bool RunParser(const wchar_t* text);

    private:
        std::vector<Pair> pairs_;
    };
}

#endif