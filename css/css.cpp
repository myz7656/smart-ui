#include "css/css.h"

#include "util/string.h"


namespace css
{
    CssDocument::CssDocument() {}

    CssDocument::~CssDocument()
    {
        Clear();
    }

    bool CssDocument::Parse(const char* text)
    {
        if (util::String::IsEmpty(text))
        {
            return false;
        }

        const unsigned char* p = (const unsigned char*)text;
        if (*(p+0) && *(p+0) == 0xefU &&
            *(p+1) && *(p+1) == 0xbbU &&
            *(p+2) && *(p+2) == 0xbfU)
        {
            text += 3;
        }

        std::wstring u_text = util::String::UTF8ToUnicode(text);
        return RunParser(u_text.c_str());
    }

    bool CssDocument::RunParser(const wchar_t* text)
    {
        if (util::String::IsEmpty(text))
        {
            return false;
        }
        const wchar_t* p = text;
        while (p && *p)
        {
            Pair pair;
            p = pair.selectors.Parse(p);

            if (!p || !*p || *p != L'{')
            {
                return false;
            }

            ++p;
            p = pair.declares.Parse(p);
            if (!p || !*p || *p != L'}')
            {
                return false;
            }

            if (pair.selectors.HasSelectors() && pair.declares.HasDeclares())
            {
                pairs_.push_back(pair);
            }
            ++p;
        }
        return true;
    }

    bool CssDocument::FindSelectorByTag(
        const std::wstring& tag_name,
        const std::wstring& pseudo_class,
        DeclareSet& declares)
    {
        bool bRet = false;

        for (std::vector<Pair>::iterator iter = pairs_.begin(); 
            iter != pairs_.end(); ++iter)
        {
            if (iter->selectors.MatchTag(tag_name.c_str(), pseudo_class.c_str()))
            {
                declares.Push(iter->declares);
                bRet = true;
            }
        }

        return bRet;
    }

    bool CssDocument::FindSelectorByClass(
        const std::wstring& class_name,
        const std::wstring& tag_name,
        const std::wstring& pseudo_class,
        DeclareSet& declares)
    {
        bool bRet = false;

        for (std::vector<Pair>::iterator iter = pairs_.begin();
            iter != pairs_.end(); ++iter)
        {
            if (iter->selectors.MatchClass(class_name.c_str(), tag_name.c_str(), pseudo_class.c_str()))
            {
                declares.Push(iter->declares);
                bRet = true;
            }
        }

        return bRet;
    }

    bool CssDocument::FindSelectorById(
        const std::wstring& id,
        const std::wstring& tag_name,
        const std::wstring& pseudo_class,
        DeclareSet& declares)
    {
        bool bRet = false;

        for (std::vector<Pair>::iterator iter = pairs_.begin(); 
            iter != pairs_.end(); ++iter)
        {
            if (iter->selectors.MatchId(id.c_str(), tag_name.c_str(), pseudo_class.c_str()))
            {
                declares.Push(iter->declares);
                bRet = true;
            }
        }

        return bRet;
    }

    void CssDocument::Clear()
    {
        for (std::vector<Pair>::iterator iter = pairs_.begin(); 
            iter != pairs_.end(); ++iter)
        {
            iter->selectors.Clear();
        }

        pairs_.clear();
    }
}