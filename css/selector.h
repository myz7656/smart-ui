#ifndef __css_selector_h__
#define __css_selector_h__

#include <string>
#include <vector>

#include "base/def.h"


namespace css
{
    class Selector
    {
    public:
        friend class SelectorDerived;
        enum Type
        {
            TYPE_ID,
            TYPE_CLASS,
            TYPE_TAG,
        };

        Selector();
        ~Selector();

        Type GetType();

        bool Result() { return result_; }

        const wchar_t* Parse(const wchar_t* text);

        bool MatchTag(const wchar_t* tag_name, const wchar_t* pseudo_class);
        bool MatchClass(const wchar_t* class_name, const wchar_t* tag_name, const wchar_t* pseudo_class);
        bool MatchId(const wchar_t* id_name, const wchar_t* tag_name, const wchar_t* pseudo_class);
        bool MatchPseudoClass( const wchar_t* pseudo_class );

    private:
        const wchar_t* ParseTag(const wchar_t* text);
        const wchar_t* ParseClass(const wchar_t* text);
        const wchar_t* ParseId(const wchar_t* text);
        const wchar_t* ParsePseudoClass(const wchar_t* text);

    private:
        std::wstring tag_;
        std::wstring value_;
        std::wstring pseudo_class_;
        Type         type_;
        bool         result_;

        Selector* next_;
        Selector* prev_;

    private:
        DISABLE_COPY_AND_ASSIGN(Selector);
    };



    class SelectorDerived
    {
    public:
        SelectorDerived();
        ~SelectorDerived();

        bool HasChild() { return first_ != 0; }

        const wchar_t* Parse(const wchar_t* text);

        bool MatchTag(const wchar_t* tag_name, const wchar_t* pseudo_class);
        bool MatchClass(const wchar_t* class_name, const wchar_t* tag_name, const wchar_t* pseudo_class);
        bool MatchId(const wchar_t* id_name, const wchar_t* tag_name, const wchar_t* pseudo_class);

        bool IsDescendantSelector();
        void Clear();

    private:
        const wchar_t* ParseSelector(const wchar_t* text);
        void LinkEnd(Selector* add);
        void LinkHead(Selector* add);

        friend class SelectorDerivedIterator;

    private:
        Selector* first_;
        Selector* last_;
    };

    class SelectorSet
    {
    public:
        SelectorSet();
        ~SelectorSet();

        bool HasSelectors();

        const wchar_t* Parse(const wchar_t* text);

        bool MatchTag(const wchar_t* tag_name, const wchar_t* pseudo_class);
        bool MatchClass(const wchar_t* class_name, const wchar_t* tag_name, const wchar_t* pseudo_class);
        bool MatchId(const wchar_t* id_name, const wchar_t* tag_name, const wchar_t* pseudo_class);

        void Clear();

    private:
        std::vector<SelectorDerived> selectors_;
    };
}

#endif