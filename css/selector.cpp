#include "css/selector.h"

#include "util/string.h"

namespace css
{
    Selector::Selector()
        : type_(TYPE_CLASS)
        , next_(0)
        , prev_(0)
        , result_(false) {}

    Selector::~Selector() {}

    const wchar_t* Selector::Parse(const wchar_t* text)
    {
        const wchar_t* p = text;
        while (p && *p)
        {
            while (iswspace(*p))
                ++p;

            if (*p == L':')
            {
                p = ParsePseudoClass(p);
            }

            if (Result())
            {
                break;
            }

            if (*p == L',' ||
                *p == L'{')
            {
                break;
            }

            if (*p == L'.')
            {
                p = ParseClass(p);
            }
            else if (*p == L'#')
            {
                p = ParseId(p);
            }
            else
            {
                p = ParseTag(p);
            }
        }

        return p;
    }

    const wchar_t* Selector::ParseTag(const wchar_t* text)
    {
        type_ = TYPE_TAG;

        const wchar_t* p = text;
        while (p && *p)
        {
            if (iswspace(*p) ||
                *p == L',' ||
                *p == L'{' ||
                *p == L':')
            {
                break;
            }

            if (*p == L'.')
            {
                p = ParseClass(p);
                break;
            }
            else if (*p == L'#')
            {
                p = ParseId(p);
                break;
            }
            else
            {
                tag_ += *p;
                ++p;
            }
        }

        if (type_ == TYPE_TAG)
        {
            result_ = !tag_.empty();
        }

        return p;
    }

    const wchar_t* Selector::ParseClass(const wchar_t* text)
    {
        type_ = TYPE_CLASS;
        const wchar_t* p = text;

        if (p)
        {
            p++;
        }

        while (p && *p)
        {
            if (iswspace(*p) ||
                *p == L',' ||
                *p == L'{' ||
                *p == L':')
            {
                break;
            }

            value_ += *p;
            ++p;
        }

        result_ = !value_.empty();

        return p;
    }

    const wchar_t* Selector::ParseId(const wchar_t* text)
    {
        type_ = TYPE_ID;
        const wchar_t* p = text;

        if (p)
        {
            p++;
        }

        while (p && *p)
        {
            if (iswspace(*p) ||
                *p == L',' ||
                *p == L'{' ||
                *p == L':')
            {
                break;
            }

            value_ += *p;
            ++p;
        }

        result_ = !value_.empty();

        return p;
    }


    const wchar_t* Selector::ParsePseudoClass(const wchar_t* text)
    {
        bool result = false;
        pseudo_class_.clear();
        const wchar_t* p = text;

        if (p)
        {
            p++;
        }

        while (p && *p)
        {
            if (result && iswspace(*p))
            {
                break;
            }

            while (iswspace(*p))
                ++p;

            if (*p == L',' ||
                *p == L'{')
            {
                break;
            }
            
            pseudo_class_ += *p;
            ++p;
            result = true;
        }

        return p;
    }


    bool Selector::MatchTag(const wchar_t* tag_name, const wchar_t* pseudo_class)
    {
        if (type_ != TYPE_TAG)
        {
            return false;
        }

        if (!tag_name || !pseudo_class)
        {
            return false;
        }

        if (tag_.compare(tag_name) == 0 &&
            MatchPseudoClass(pseudo_class))
        {
            return true;
        }

        return false;
    }

    bool Selector::MatchClass(const wchar_t* class_name, const wchar_t* tag_name, const wchar_t* pseudo_class)
    {
        if (type_ != TYPE_CLASS)
        {
            return false;
        }

        if (!class_name|| !pseudo_class)
        {
            return false;
        }

        if (value_.compare(class_name) == 0 && 
            (tag_.empty() || tag_.compare(tag_name) == 0) &&
            MatchPseudoClass(pseudo_class))
        {
            return true;
        }

        return false;
    }

    bool Selector::MatchId(const wchar_t* id_name, const wchar_t* tag_name, const wchar_t* pseudo_class)
    {
        if (type_ != TYPE_ID)
        {
            return false;
        }

        if (!id_name || !pseudo_class)
        {
            return false;
        }

        if (value_.compare(id_name) == 0 &&
            (tag_.empty() || tag_.compare(tag_name) == 0) &&
            MatchPseudoClass(pseudo_class))
        {
            return true;
        }

        return false;
    }

    bool Selector::MatchPseudoClass(const wchar_t* pseudo_class)
    {
        return (pseudo_class_.empty() && (_tcscmp(pseudo_class, _T("normal")) == 0))
            || (pseudo_class_.compare(pseudo_class) == 0);
    }

    SelectorDerived::SelectorDerived()
        : first_(0)
        , last_(0) {}

    SelectorDerived::~SelectorDerived() {}

    const wchar_t* SelectorDerived::Parse(const wchar_t* text)
    {
        const wchar_t* p = text;
        while (p && *p)
        {
            if (*p == L',' ||
                *p == L'{')
            {
                break;
            }

            p = ParseSelector(p);
        }

        return p;
    }

    const wchar_t* SelectorDerived::ParseSelector(const wchar_t* text)
    {
        const wchar_t* p = text;
        Selector* selector = new Selector;
        if (!selector)
        {
            return p;
        }
        p = selector->Parse(p);
        if (selector->Result())
        {
            LinkHead(selector);
        }
        else
        {
            delete selector;
        }
        return p;
    }

    void SelectorDerived::LinkEnd(Selector* add)
    {
        if (!add)
        {
            return;
        }

        add->prev_ = last_;
        add->next_ = 0;

        if (last_)
            last_->next_ = add;
        else
            first_ = add;

        last_ = add;
    }

    void SelectorDerived::LinkHead(Selector* add)
    {
        if (!add)
        {
            return ;
        }

        add->prev_ = 0;
        add->next_ = first_;

        if (first_)
        {
            first_->prev_ = add;
        }
        else
        {
            last_ = add;
        }

        first_ = add;
    }

    void SelectorDerived::Clear()
    {
        Selector* s = first_;
        while (s)
        {
            Selector* next = s->next_;
            delete s;
            s = next;
        }
        first_ = 0;
        last_ = 0;
    }

    bool SelectorDerived::MatchTag(const wchar_t* tag_name, const wchar_t* pseudo_class)
    {
        return first_ && first_->MatchTag(tag_name, pseudo_class);
    }

    bool SelectorDerived::MatchClass(const wchar_t* class_name, const wchar_t* tag_name, const wchar_t* pseudo_class)
    {
        return first_ && first_->MatchClass(class_name, tag_name, pseudo_class);
    }

    bool SelectorDerived::MatchId(const wchar_t* id_name, const wchar_t* tag_name, const wchar_t* pseudo_class)
    {
        return first_ && first_->MatchId(id_name, tag_name, pseudo_class);
    }

    bool SelectorDerived::IsDescendantSelector()
    {
        return first_ != last_;
    }


    SelectorSet::SelectorSet() {}

    SelectorSet::~SelectorSet() {}

    bool SelectorSet::HasSelectors()
    {
        return selectors_.size() > 0;
    }

    const wchar_t* SelectorSet::Parse(const wchar_t* text)
    {
        const wchar_t* p = text;
        while (p && *p)
        {
            if (iswspace(*p))
            {
                ++p;
                continue;
            }

            if (*p == L'{')
            {
                break;
            }

            SelectorDerived selector;
            p = selector.Parse(p);
            if (!p || !*p || (*p != L',' && *p != L'{'))
            {
                break;
            }
            if (*p == L',')
            {
                ++p;
            }

            if (selector.HasChild())
            {
                selectors_.push_back(selector);
            }
        }
        return p;
    }

    bool SelectorSet::MatchTag(const wchar_t* tag_name, const wchar_t* pseudo_class)
    {
        for (std::vector<SelectorDerived>::iterator iter = selectors_.begin();
            iter != selectors_.end(); ++iter)
        {
            if (iter->MatchTag(tag_name, pseudo_class))
            {
                return true;
            }
        }

        return false;
    }

    bool SelectorSet::MatchClass(const wchar_t* class_name, const wchar_t* tag_name, const wchar_t* pseudo_class)
    {
        for (std::vector<SelectorDerived>::iterator iter = selectors_.begin();
            iter != selectors_.end(); ++iter)
        {
            if (iter->MatchClass(class_name, tag_name, pseudo_class))
            {
                return true;
            }
        }

        return false;
    }

    bool SelectorSet::MatchId(const wchar_t* id_name, const wchar_t* tag_name, const wchar_t* pseudo_class)
    {
        for (std::vector<SelectorDerived>::iterator iter = selectors_.begin();
            iter != selectors_.end(); ++iter)
        {
            if (iter->MatchId(id_name, tag_name, pseudo_class))
            {
                return true;
            }
        }

        return false;
    }

    void SelectorSet::Clear()
    {
        for (std::vector<SelectorDerived>::iterator iter = selectors_.begin();
            iter != selectors_.end(); ++iter)
        {
            iter->Clear();
        }

        selectors_.clear();
    }
}