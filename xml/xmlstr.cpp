#include "xml/xmlstr.h"

namespace xml
{



const XmlString::size_type XmlString::npos = static_cast< XmlString::size_type >(-1);

XmlString::Rep XmlString::nullrep_ = { 0, 0, { '\0' } };

void XmlString::reserve (size_type cap)
{
    if (cap > capacity())
    {
        XmlString tmp;
        tmp.init(length(), cap);
        memcpy(tmp.start(), data(), length());
        swap(tmp);
    }
}

XmlString& XmlString::assign(const char* str, size_type len)
{
    size_type cap = capacity();
    if (len > cap || cap > 3*(len + 8))
    {
        XmlString tmp;
        tmp.init(len);
        memcpy(tmp.start(), str, len);
        swap(tmp);
    }
    else
    {
        memmove(start(), str, len);
        set_size(len);
    }
    return *this;
}

XmlString& XmlString::append(const char* str, size_type len)
{
    size_type newsize = length() + len;
    if (newsize > capacity())
    {
        reserve (newsize + capacity());
    }
    memmove(finish(), str, len);
    set_size(newsize);
    return *this;
}

XmlString operator + (const XmlString & a, const XmlString & b)
{
    XmlString tmp;
    tmp.reserve(a.length() + b.length());
    tmp += a;
    tmp += b;
    return tmp;
}

XmlString operator + (const XmlString & a, const char* b)
{
    XmlString tmp;
    XmlString::size_type b_len = static_cast<XmlString::size_type>( strlen(b) );
    tmp.reserve(a.length() + b_len);
    tmp += a;
    tmp.append(b, b_len);
    return tmp;
}

XmlString operator + (const char* a, const XmlString & b)
{
    XmlString tmp;
    XmlString::size_type a_len = static_cast<XmlString::size_type>( strlen(a) );
    tmp.reserve(a_len + b.length());
    tmp.append(a, a_len);
    tmp += b;
    return tmp;
}



}