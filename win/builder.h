#ifndef __win_builder_h__
#define __win_builder_h__

#include <windows.h>

#include "base/def.h"

#include "xml/xml.h"
#include "css/css.h"

namespace core
{
    class StStyleCache;
    class StWidget;
    class StContainer;
    class StStyle;
}

namespace css
{
    class DeclareSet;
}

namespace win
{
    class Builder
    {
    public:
        Builder() {}
        ~Builder() {}

        core::StWidget* Parse(const wchar_t* file);
        core::StWidget* ParseText(const wchar_t* text);

    private:
        BOOL BuildHeader(xml::XmlElement* root);
        core::StWidget* BuildBody(xml::XmlElement* root);
        core::StWidget* BuildNode(xml::XmlElement* element, core::StWidget* containter= NULL);

    private:
        bool ParseAttribute(xml::XmlElement* element, core::StWidget* widget);
        bool BuildChildren(xml::XmlElement *element, core::StContainer* containter);
        bool UpdateStyle(core::StWidget* widget, const std::string& value, const std::string& classname, DWORD state);

        void AddDeclareSetToStyle(css::DeclareSet& declare_set, core::StStyle& style);
        std::wstring ConverStateToPseudoClass(DWORD state);

    private:
        css::CssDocument style_sheet_;

    private:
        DISABLE_COPY_AND_ASSIGN(Builder);
    };
}

#endif