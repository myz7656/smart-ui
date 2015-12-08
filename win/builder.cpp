#include "win/builder.h"

#include "base/scoped_ptr.h"
#include "base/singleton.h"

#include "core/def.h"
#include "core/style.h"
#include "core/widget.h"
#include "core/container.h"

#include "win/asset_manager.h"

#include "util/string.h"
#include "core/controls/span.h"

namespace win
{
    core::StWidget* Builder::Parse(const wchar_t *file)
    {
        if (util::String::IsEmpty<wchar_t>(file))
        {
            return 0;
        }

        base::ScopedArray<BYTE> byte_;

        if (!AssetManagerSingleton::Instance().LoadBinary(file, byte_.accept(), 0))
        {
            return 0;
        }

        xml::XmlDocument doc;
        if (!doc.Parse((const char *)byte_.get()) || doc.Error())
        {
            return 0;
        }

        xml::XmlElement* root = doc.RootElement();
        if (!root)
        {
            return 0;
        }

        const char* value = root->Value();
        if (!value || strcmp(value, "widget") != 0)
        {
            return 0;
        }

        BuildHeader(root);
        return BuildBody(root);
    }

    core::StWidget* Builder::ParseText(const wchar_t* text)
    {
        if (util::String::IsEmpty<wchar_t>(text))
        {
            return 0;
        }

        std::string xml = util::String::UnicodeToUTF8(text);
        xml::XmlDocument doc;
        doc.Parse(xml.c_str());
        if (doc.Error())
        {
            return 0;
        }

        xml::XmlElement* element = doc.RootElement();
        if (!element)
        {
            return 0;
        }

        return BuildNode(element);
    }

    BOOL Builder::BuildHeader(xml::XmlElement* root)
    {
        if (!root)
        {
            return FALSE;
        }

        xml::XmlElement* head = root->FirstChildElement("head");
        if (!head)
        {
            return FALSE;
        }

        xml::XmlElement* style = head->FirstChildElement("style");
        if (style)
        {
            const char* type = style->Attribute("type");
            if (type && strcmp(type, "text/css") == 0)
            {
                style_sheet_.Parse(style->GetText());
            }
        }

        xml::XmlElement* link = head->FirstChildElement("link");
        while (link)
        {
            const char* type = link->Attribute("type");
            if (type && strcmp(link->Attribute("type"), "text/css") == 0)
            {
                const char* file = link->Attribute("href");
                std::wstring path = util::String::UTF8ToUnicode(file);

                base::ScopedArray<BYTE> byte_;

                if (AssetManagerSingleton::Instance().LoadBinary(path.c_str(), byte_.accept(), 0))
                {
                    style_sheet_.Parse((const char *)byte_.get());
                }
            }

            link = link->NextSiblingElement("link");
        }

        return TRUE;
    }

    core::StWidget* Builder::BuildBody(xml::XmlElement* root)
    {
        if (!root)
        {
            return 0;
        }

        xml::XmlElement* body = root->FirstChildElement("body");
        if (!body)
        {
            return 0;
        }
        core::StContainer* body_ = new core::StContainer;
        if (body_)
        {
            body_->SetTagName("body");
            if (!ParseAttribute(body, body_))
            {
                body_->Drop();
                return 0;
            }

            BuildChildren(body, body_);

            return static_cast<core::StWidget*>(body_);
        }

        return 0;
    }

    core::StWidget* Builder::BuildNode(xml::XmlElement* element, core::StWidget* containter)
    {
        if (!element)
        {
            return 0;
        }

        const char * name = element->Value();
        core::StWidget* widget = (core::StWidget*)RuntimeClass::CreateObject(name);

        if (widget)
        {
            widget->SetTagName(name);

            if (containter)
            {
                widget->Inherit(*containter);
            }

            if (!ParseAttribute(element, widget))
            {
                widget->Drop();
                return 0;
            }

            if (widget->IsContainer())
            {
                BuildChildren(element, (core::StContainer*)widget);
            }
            return widget;
        }

        return 0;
    }

    bool Builder::ParseAttribute(xml::XmlElement* element, core::StWidget* widget)
    {
        std::string style_normal, style_hover, style_down;
        std::string class_normal, class_hover, class_down;

        const xml::XmlAttribute *attribute = element->FirstAttribute();
        while (attribute)
        {
            const char *value = attribute->Value();
            const char *name = attribute->Name();
            if (!name)
            {
                continue;
            }

            if (strcmp("style", name) == 0)
            {
                style_normal = value;
            }
            else if (strcmp("class", name) == 0)
            {
                class_normal = value;
            }
            else if (strcmp("hover-style", name) == 0)
            {
                style_hover = value;
            }
            else if (strcmp("down-style", name) == 0)
            {
                style_down = value;
            }
            else
            {
                widget->SetAttribute(name, value, 0);
            }

            attribute = attribute->Next();
        }

        const char* text = element->GetText();
        if (text)
        {
            widget->SetAttribute(core::WIDGET_PROPERTY_INNER_TEXT, text, 0);
        }

        UpdateStyle(widget, style_normal, class_normal, core::ACTION_STATE_NORMAL);
        UpdateStyle(widget, style_hover, class_normal, core::ACTION_STATE_HOVER);
        UpdateStyle(widget, style_down, class_normal, core::ACTION_STATE_DOWN);

        return true;
    }

    bool Builder::BuildChildren(xml::XmlElement *element, core::StContainer* containter)
    {
        xml::XmlNode *node = 0;
        while (0 != (node = element->IterateChildren(node)))
        {
            if (!node->Value())
            {
                continue;
            }

            if (xml::XmlNode::XML_ELEMENT == node->Type())
            {
                xml::XmlElement* child = node->ToElement();
                const char* name = node->Value();
                if (!name || containter->IsFilterWidget(name))
                {
                    continue;
                }
                
                core::StWidget* widget = BuildNode(child, containter);
                if (!widget)
                {
                    continue;
                }

                containter->LinkEnd(widget);
            }
        }

        return true;
    }

    bool Builder::UpdateStyle(
        core::StWidget* widget,
        const std::string& value,
        const std::string& classname,
        DWORD state)
    {
        if (!widget)
        {
            return false;
        }

        core::StStyle& style_noraml = widget->GetStyle(core::ACTION_STATE_NORMAL);
        core::StStyle& style = widget->GetStyle(state);
        if (state == core::ACTION_STATE_HOVER ||
            state == core::ACTION_STATE_DOWN)
        {
            style.Copy(style_noraml);
        }

        css::DeclareSet declare_set;
        std::wstring pseudo_class = ConverStateToPseudoClass(state);
        std::wstring tag_name = util::String::UTF8ToUnicode(widget->TagName().c_str());

        declare_set.Clear();
        style_sheet_.FindSelectorByTag(tag_name, pseudo_class, declare_set);

        if (declare_set.HasDeclares())
        {
            AddDeclareSetToStyle(declare_set, style);
        }

        if (!classname.empty())
        {
            declare_set.Clear();
            style_sheet_.FindSelectorByClass(util::String::UTF8ToUnicode(classname.c_str()), tag_name, pseudo_class, declare_set);
            if (declare_set.HasDeclares())
            {
                AddDeclareSetToStyle(declare_set, style);
            }
        }

        std::string id = "";
        widget->GetAttribute<std::string>(core::WIDGET_PROPERTY_ID, &id);

        if (!id.empty())
        {
            declare_set.Clear();
            style_sheet_.FindSelectorById(util::String::UTF8ToUnicode(id.c_str()), tag_name, pseudo_class, declare_set);
            if (declare_set.HasDeclares())
            {
                AddDeclareSetToStyle(declare_set, style);
            }
        }

        if (!value.empty())
        {
            style.ParseValues(value.c_str());
        }

        return true;
    }

    void Builder::AddDeclareSetToStyle(css::DeclareSet& declare_set, core::StStyle& style)
    {
        std::vector<css::Declare>& vecDeclare = declare_set.Declares();
        for (std::vector<css::Declare>::iterator iter = vecDeclare.begin(); 
            iter != vecDeclare.end(); ++iter)
        {
            style.SetAttribute(util::String::UnicodeToUTF8(iter->Property().c_str()).c_str(),
                util::String::UnicodeToUTF8(iter->Value().c_str()).c_str(), NULL);
        }
    }

    std::wstring Builder::ConverStateToPseudoClass(DWORD state)
    {
        std::wstring strPseudoClass;

        switch (state)
        {
        case core::ACTION_STATE_NORMAL:
            strPseudoClass = _T("normal");
            break;

        case core::ACTION_STATE_HOVER:
            strPseudoClass = _T("hover");
            break;

        case core::ACTION_STATE_DOWN:
            strPseudoClass = _T("down");
            break;

        default:
            break;
        }

        return strPseudoClass;
    }
}