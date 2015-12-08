#include <ctype.h>
#include <stddef.h>

#include "xml/xml.h"

namespace xml
{



XmlBase::Entity XmlBase::entity[ XmlBase::NUM_ENTITY ] = 
{
    { "&amp;",  5, '&' },
    { "&lt;",   4, '<' },
    { "&gt;",   4, '>' },
    { "&quot;", 6, '\"' },
    { "&apos;", 6, '\'' }
};

const unsigned char XML_UTF_LEAD_0 = 0xefU;
const unsigned char XML_UTF_LEAD_1 = 0xbbU;
const unsigned char XML_UTF_LEAD_2 = 0xbfU;

const int XmlBase::utf8ByteTable[256] = 
{
    1,    1,    1,    1,    1,    1,    1,    1,    1,    1,    1,    1,    1,    1,    1,    1,
    1,    1,    1,    1,    1,    1,    1,    1,    1,    1,    1,    1,    1,    1,    1,    1,
    1,    1,    1,    1,    1,    1,    1,    1,    1,    1,    1,    1,    1,    1,    1,    1,
    1,    1,    1,    1,    1,    1,    1,    1,    1,    1,    1,    1,    1,    1,    1,    1,
    1,    1,    1,    1,    1,    1,    1,    1,    1,    1,    1,    1,    1,    1,    1,    1,
    1,    1,    1,    1,    1,    1,    1,    1,    1,    1,    1,    1,    1,    1,    1,    1,
    1,    1,    1,    1,    1,    1,    1,    1,    1,    1,    1,    1,    1,    1,    1,    1,
    1,    1,    1,    1,    1,    1,    1,    1,    1,    1,    1,    1,    1,    1,    1,    1,
    1,    1,    1,    1,    1,    1,    1,    1,    1,    1,    1,    1,    1,    1,    1,    1,
    1,    1,    1,    1,    1,    1,    1,    1,    1,    1,    1,    1,    1,    1,    1,    1,
    1,    1,    1,    1,    1,    1,    1,    1,    1,    1,    1,    1,    1,    1,    1,    1,
    1,    1,    1,    1,    1,    1,    1,    1,    1,    1,    1,    1,    1,    1,    1,    1,
    1,    1,    2,    2,    2,    2,    2,    2,    2,    2,    2,    2,    2,    2,    2,    2,
    2,    2,    2,    2,    2,    2,    2,    2,    2,    2,    2,    2,    2,    2,    2,    2,
    3,    3,    3,    3,    3,    3,    3,    3,    3,    3,    3,    3,    3,    3,    3,    3,
    4,    4,    4,    4,    4,    1,    1,    1,    1,    1,    1,    1,    1,    1,    1,    1
};


void XmlBase::ConvertUTF32ToUTF8( unsigned long input, char* output, int* length )
{
    const unsigned long BYTE_MASK = 0xBF;
    const unsigned long BYTE_MARK = 0x80;
    const unsigned long FIRST_BYTE_MARK[7] = { 0x00, 0x00, 0xC0, 0xE0, 0xF0, 0xF8, 0xFC };

    if (input < 0x80) 
        *length = 1;
    else if ( input < 0x800 )
        *length = 2;
    else if ( input < 0x10000 )
        *length = 3;
    else if ( input < 0x200000 )
        *length = 4;
    else
        { *length = 0; return; }

    output += *length;

    switch (*length) 
    {
        case 4:
            --output; 
            *output = (char)((input | BYTE_MARK) & BYTE_MASK); 
            input >>= 6;
        case 3:
            --output; 
            *output = (char)((input | BYTE_MARK) & BYTE_MASK); 
            input >>= 6;
        case 2:
            --output; 
            *output = (char)((input | BYTE_MARK) & BYTE_MASK); 
            input >>= 6;
        case 1:
            --output; 
            *output = (char)(input | FIRST_BYTE_MARK[*length]);
    }
}


int XmlBase::IsAlpha( unsigned char anyByte, XmlEncoding)
{
    if ( anyByte < 127 )
        return isalpha( anyByte );
    else
        return 1;
}


int XmlBase::IsAlphaNum( unsigned char anyByte, XmlEncoding)
{
    if ( anyByte < 127 )
        return isalnum( anyByte );
    else
        return 1;
}

class XmlParsingData
{
    friend class XmlDocument;

public:
    void Stamp( const char* now, XmlEncoding encoding );
    const XmlCursor& Cursor() const    { return cursor; }

private:
    XmlParsingData( const char* start, int _tabsize, int row, int col )
    {
        assert( start );
        stamp = start;
        tabsize = _tabsize;
        cursor.row = row;
        cursor.col = col;
    }

    XmlCursor        cursor;
    const char*        stamp;
    int                tabsize;
};

void XmlParsingData::Stamp( const char* now, XmlEncoding encoding )
{
    assert( now );

    if ( tabsize < 1 )
    {
        return;
    }

    int row = cursor.row;
    int col = cursor.col;
    const char* p = stamp;
    assert( p );

    while ( p < now )
    {
        const unsigned char* pU = (const unsigned char*)p;
        switch (*pU) {
            case 0:
                return;

            case '\r':
                ++row;
                col = 0;
                ++p;
                if (*p == '\n') {
                    ++p;
                }
                break;

            case '\n':
                ++row;
                col = 0;
                ++p;
                if (*p == '\r') {
                    ++p;
                }
                break;

            case '\t':
                ++p;
                col = (col / tabsize + 1) * tabsize;
                break;

            case XML_UTF_LEAD_0:
                if ( encoding == XML_ENCODING_UTF8 )
                {
                    if ( *(p+1) && *(p+2) )
                    {
                        if ( *(pU+1)==XML_UTF_LEAD_1 && *(pU+2)==XML_UTF_LEAD_2 )
                            p += 3;
                        else if ( *(pU+1)==0xbfU && *(pU+2)==0xbeU )
                            p += 3;
                        else if ( *(pU+1)==0xbfU && *(pU+2)==0xbfU )
                            p += 3;
                        else
                            { p +=3; ++col; }
                    }
                }
                else
                {
                    ++p;
                    ++col;
                }
                break;

            default:
                if ( encoding == XML_ENCODING_UTF8 )
                {
                    int step = XmlBase::utf8ByteTable[*((const unsigned char*)p)];
                    if ( step == 0 )
                        step = 1;
                    p += step;

                    ++col;
                }
                else
                {
                    ++p;
                    ++col;
                }
                break;
        }
    }
    cursor.row = row;
    cursor.col = col;
    assert( cursor.row >= -1 );
    assert( cursor.col >= -1 );
    stamp = p;
    assert( stamp );
}

const char* XmlBase::SkipWhiteSpace( const char* p, XmlEncoding encoding )
{
    if ( !p || !*p )
    {
        return 0;
    }
    if ( encoding == XML_ENCODING_UTF8 )
    {
        while ( *p )
        {
            const unsigned char* pU = (const unsigned char*)p;
            if (    *(pU+0)==XML_UTF_LEAD_0
                 && *(pU+1)==XML_UTF_LEAD_1 
                 && *(pU+2)==XML_UTF_LEAD_2 )
            {
                p += 3;
                continue;
            }
            else if(*(pU+0)==XML_UTF_LEAD_0
                 && *(pU+1)==0xbfU
                 && *(pU+2)==0xbeU )
            {
                p += 3;
                continue;
            }
            else if(*(pU+0)==XML_UTF_LEAD_0
                 && *(pU+1)==0xbfU
                 && *(pU+2)==0xbfU )
            {
                p += 3;
                continue;
            }

            if ( IsWhiteSpace( *p ) )
                ++p;
            else
                break;
        }
    }
    else
    {
        while ( *p && IsWhiteSpace( *p ) )
            ++p;
    }

    return p;
}


const char* XmlBase::ReadName( const char* p, XmlString * name, XmlEncoding encoding )
{
    *name = "";
    assert( p );

    if (    p && *p 
         && ( IsAlpha( (unsigned char) *p, encoding ) || *p == '_' ) )
    {
        const char* start = p;
        while(    p && *p
               && (    IsAlphaNum( (unsigned char ) *p, encoding )
                    || *p == '_'
                    || *p == '-'
                    || *p == '.'
                    || *p == ':' ) )
        {
            ++p;
        }
        if ( p-start > 0 ) {
            name->assign( start, p-start );
        }
        return p;
    }
    return 0;
}

const char* XmlBase::GetEntity( const char* p, char* value, int* length, XmlEncoding encoding )
{
    XmlString ent;
    int i;
    *length = 0;

    if ( *(p+1) && *(p+1) == '#' && *(p+2) )
    {
        unsigned long ucs = 0;
        ptrdiff_t delta = 0;
        unsigned mult = 1;

        if ( *(p+2) == 'x' )
        {
            if ( !*(p+3) ) return 0;

            const char* q = p+3;
            q = strchr( q, ';' );

            if ( !q || !*q ) return 0;

            delta = q-p;
            --q;

            while ( *q != 'x' )
            {
                if ( *q >= '0' && *q <= '9' )
                    ucs += mult * (*q - '0');
                else if ( *q >= 'a' && *q <= 'f' )
                    ucs += mult * (*q - 'a' + 10);
                else if ( *q >= 'A' && *q <= 'F' )
                    ucs += mult * (*q - 'A' + 10 );
                else 
                    return 0;
                mult *= 16;
                --q;
            }
        }
        else
        {
            if ( !*(p+2) ) return 0;

            const char* q = p+2;
            q = strchr( q, ';' );

            if ( !q || !*q ) return 0;

            delta = q-p;
            --q;

            while ( *q != '#' )
            {
                if ( *q >= '0' && *q <= '9' )
                    ucs += mult * (*q - '0');
                else 
                    return 0;
                mult *= 10;
                --q;
            }
        }
        if ( encoding == XML_ENCODING_UTF8 )
        {
            ConvertUTF32ToUTF8( ucs, value, length );
        }
        else
        {
            *value = (char)ucs;
            *length = 1;
        }
        return p + delta + 1;
    }

    for( i=0; i<NUM_ENTITY; ++i )
    {
        if ( strncmp( entity[i].str, p, entity[i].strLength ) == 0 )
        {
            assert( strlen( entity[i].str ) == entity[i].strLength );
            *value = entity[i].chr;
            *length = 1;
            return ( p + entity[i].strLength );
        }
    }

    *value = *p;

    return p+1;
}

bool XmlBase::StringEqual( const char* p,
                             const char* tag,
                             bool ignoreCase,
                             XmlEncoding encoding )
{
    assert( p );
    assert( tag );
    if ( !p || !*p )
    {
        assert( 0 );
        return false;
    }

    const char* q = p;

    if ( ignoreCase )
    {
        while ( *q && *tag && ToLower( *q, encoding ) == ToLower( *tag, encoding ) )
        {
            ++q;
            ++tag;
        }

        if ( *tag == 0 )
            return true;
    }
    else
    {
        while ( *q && *tag && *q == *tag )
        {
            ++q;
            ++tag;
        }

        if ( *tag == 0 )
            return true;
    }
    return false;
}

const char* XmlBase::ReadText( const char* p,
                                 XmlString * text,
                                 bool trimWhiteSpace,
                                 const char* endTag,
                                 bool caseInsensitive,
                                 XmlEncoding encoding )
{
    *text = "";
    if (    !trimWhiteSpace
         || !condenseWhiteSpace )
    {
        while (    p && *p
                && !StringEqual( p, endTag, caseInsensitive, encoding )
              )
        {
            int len;
            char cArr[4] = { 0, 0, 0, 0 };
            p = GetChar( p, cArr, &len, encoding );
            text->append( cArr, len );
        }
    }
    else
    {
        bool whitespace = false;

        p = SkipWhiteSpace( p, encoding );
        while (    p && *p
                && !StringEqual( p, endTag, caseInsensitive, encoding ) )
        {
            if ( *p == '\r' || *p == '\n' )
            {
                whitespace = true;
                ++p;
            }
            else if ( IsWhiteSpace( *p ) )
            {
                whitespace = true;
                ++p;
            }
            else
            {
                if ( whitespace )
                {
                    (*text) += ' ';
                    whitespace = false;
                }
                int len;
                char cArr[4] = { 0, 0, 0, 0 };
                p = GetChar( p, cArr, &len, encoding );
                if ( len == 1 )
                    (*text) += cArr[0];
                else
                    text->append( cArr, len );
            }
        }
    }
    if ( p && *p )
        p += strlen( endTag );
    return ( p && *p ) ? p : 0;
}

const char* XmlBase::ReadText( const char* p,
                               XmlString * text,
                               bool trimWhiteSpace,
                               bool keepSingleWhiteSpace,
                               const char* endTag,
                               bool caseInsensitive,
                               XmlEncoding encoding )
{
    *text = "";
    if (    !trimWhiteSpace
        || !condenseWhiteSpace )
    {
        while (    p && *p
            && !StringEqual( p, endTag, caseInsensitive, encoding )
            )
        {
            int len;
            char cArr[4] = { 0, 0, 0, 0 };
            p = GetChar( p, cArr, &len, encoding );
            text->append( cArr, len );
        }
    }
    else
    {
        bool whitespace = false;

        p = SkipWhiteSpace( p, encoding );
        while (    p && *p
            && !StringEqual( p, endTag, caseInsensitive, encoding ) )
        {
            if ( *p == '\r' || *p == '\n' )
            {
                whitespace = true;
                ++p;
            }
            else if ( IsWhiteSpace( *p ) )
            {
                whitespace = true;
                ++p;
            }
            else
            {
                if ( whitespace && keepSingleWhiteSpace )
                {
                    (*text) += ' ';
                    whitespace = false;
                }
                int len;
                char cArr[4] = { 0, 0, 0, 0 };
                p = GetChar( p, cArr, &len, encoding );
                if ( len == 1 )
                    (*text) += cArr[0];
                else
                    text->append( cArr, len );
            }
        }
    }
    if ( p && *p )
        p += strlen( endTag );
    return ( p && *p ) ? p : 0;
}

const char* XmlDocument::Parse( const char* p, XmlParsingData* prevData, XmlEncoding encoding )
{
    ClearError();

    if ( !p || !*p )
    {
        SetError( XML_ERROR_DOCUMENT_EMPTY, 0, 0, XML_ENCODING_UNKNOWN );
        return 0;
    }

    location.Clear();
    if ( prevData )
    {
        location.row = prevData->cursor.row;
        location.col = prevData->cursor.col;
    }
    else
    {
        location.row = 0;
        location.col = 0;
    }
    XmlParsingData data( p, TabSize(), location.row, location.col );
    location = data.Cursor();

    if ( encoding == XML_ENCODING_UNKNOWN )
    {
        const unsigned char* pU = (const unsigned char*)p;
        if (    *(pU+0) && *(pU+0) == XML_UTF_LEAD_0
             && *(pU+1) && *(pU+1) == XML_UTF_LEAD_1
             && *(pU+2) && *(pU+2) == XML_UTF_LEAD_2 )
        {
            encoding = XML_ENCODING_UTF8;
            useMicrosoftBOM = true;
        }
    }

    p = SkipWhiteSpace( p, encoding );
    if ( !p )
    {
        SetError( XML_ERROR_DOCUMENT_EMPTY, 0, 0, XML_ENCODING_UNKNOWN );
        return 0;
    }

    while ( p && *p )
    {
        XmlNode* node = Identify( p, encoding );
        if ( node )
        {
            p = node->Parse( p, &data, encoding );
            LinkEndChild( node );
        }
        else
        {
            break;
        }

        
        if (    encoding == XML_ENCODING_UNKNOWN
             && node->ToDeclaration() )
        {
            XmlDeclaration* dec = node->ToDeclaration();
            const char* enc = dec->Encoding();
            assert( enc );

            if ( *enc == 0 )
                encoding = XML_ENCODING_UTF8;
            else if ( StringEqual( enc, "UTF-8", true, XML_ENCODING_UNKNOWN ) )
                encoding = XML_ENCODING_UTF8;
            else if ( StringEqual( enc, "UTF8", true, XML_ENCODING_UNKNOWN ) )
                encoding = XML_ENCODING_UTF8;    
            else 
                encoding = XML_ENCODING_LEGACY;
        }

        p = SkipWhiteSpace( p, encoding );
    }

    if ( !firstChild ) {
        SetError( XML_ERROR_DOCUMENT_EMPTY, 0, 0, encoding );
        return 0;
    }

    return p;
}

void XmlDocument::SetError( int err, const char* pError, XmlParsingData* data, XmlEncoding encoding )
{
    if ( error )
        return;

    assert( err > 0 && err < XML_ERROR_STRING_COUNT );
    error   = true;
    errorId = err;
    errorDesc = errorString[ errorId ];

    errorLocation.Clear();
    if ( pError && data )
    {
        data->Stamp( pError, encoding );
        errorLocation = data->Cursor();
    }
}

XmlNode* XmlNode::Identify( const char* p, XmlEncoding encoding )
{
    XmlNode* returnNode = 0;

    p = SkipWhiteSpace( p, encoding );
    if( !p || !*p || *p != '<' )
    {
        return 0;
    }

    p = SkipWhiteSpace( p, encoding );

    if ( !p || !*p )
    {
        return 0;
    }

    const char* xmlHeader = { "<?xml" };
    const char* commentHeader = { "<!--" };
    const char* dtdHeader = { "<!" };
    const char* cdataHeader = { "<![CDATA[" };

    if ( StringEqual( p, xmlHeader, true, encoding ) )
    {
        returnNode = new XmlDeclaration();
    }
    else if ( StringEqual( p, commentHeader, false, encoding ) )
    {
        returnNode = new XmlComment();
    }
    else if ( StringEqual( p, cdataHeader, false, encoding ) )
    {
        XmlText* text = new XmlText( "" );
        text->SetCDATA( true );
        returnNode = text;
    }
    else if ( StringEqual( p, dtdHeader, false, encoding ) )
    {
        returnNode = new XmlUnknown();
    }
    else if (    IsAlpha( *(p+1), encoding )
              || *(p+1) == '_' )
    {
        returnNode = new XmlElement( "" );
    }
    else
    {
        returnNode = new XmlUnknown();
    }

    if ( returnNode )
    {
        returnNode->parent = this;
    }
    return returnNode;
}


const char* XmlElement::Parse( const char* p, XmlParsingData* data, XmlEncoding encoding )
{
    p = SkipWhiteSpace( p, encoding );
    XmlDocument* document = GetDocument();

    if ( !p || !*p )
    {
        if ( document ) document->SetError( XML_ERROR_PARSING_ELEMENT, 0, 0, encoding );
        return 0;
    }

    if ( data )
    {
        data->Stamp( p, encoding );
        location = data->Cursor();
    }

    if ( *p != '<' )
    {
        if ( document ) document->SetError( XML_ERROR_PARSING_ELEMENT, p, data, encoding );
        return 0;
    }

    p = SkipWhiteSpace( p+1, encoding );

    const char* pErr = p;

    p = ReadName( p, &value, encoding );
    if ( !p || !*p )
    {
        if ( document )    document->SetError( XML_ERROR_FAILED_TO_READ_ELEMENT_NAME, pErr, data, encoding );
        return 0;
    }

    XmlString endTag ("</");
    endTag += value;

    while ( p && *p )
    {
        pErr = p;
        p = SkipWhiteSpace( p, encoding );
        if ( !p || !*p )
        {
            if ( document ) document->SetError( XML_ERROR_READING_ATTRIBUTES, pErr, data, encoding );
            return 0;
        }
        if ( *p == '/' )
        {
            ++p;

            if ( *p  != '>' )
            {
                if ( document ) document->SetError( XML_ERROR_PARSING_EMPTY, p, data, encoding );
                return 0;
            }
            return (p+1);
        }
        else if ( *p == '>' )
        {
            ++p;
            p = ReadValue( p, data, encoding );
            if ( !p || !*p ) {
                if ( document ) document->SetError( XML_ERROR_READING_END_TAG, p, data, encoding );
                return 0;
            }

            if ( StringEqual( p, endTag.c_str(), false, encoding ) )
            {
                p += endTag.length();
                p = SkipWhiteSpace( p, encoding );
                if ( p && *p && *p == '>' ) {
                    ++p;
                    return p;
                }
                if ( document ) document->SetError( XML_ERROR_READING_END_TAG, p, data, encoding );
                return 0;
            }
            else
            {
                if ( document ) document->SetError( XML_ERROR_READING_END_TAG, p, data, encoding );
                return 0;
            }
        }
        else
        {
            XmlAttribute* attrib = new XmlAttribute();
            if ( !attrib )
            {
                return 0;
            }

            attrib->SetDocument( document );
            pErr = p;
            p = attrib->Parse( p, data, encoding );

            if ( !p || !*p )
            {
                if ( document ) document->SetError( XML_ERROR_PARSING_ELEMENT, pErr, data, encoding );
                delete attrib;
                return 0;
            }

            XmlAttribute* node = attributeSet.Find( attrib->Name() );
            if ( node )
            {
                if ( document ) document->SetError( XML_ERROR_PARSING_ELEMENT, pErr, data, encoding );
                delete attrib;
                return 0;
            }

            attributeSet.Add( attrib );
        }
    }

    return p;
}


const char* XmlElement::ReadValue( const char* p, XmlParsingData* data, XmlEncoding encoding )
{
    XmlDocument* document = GetDocument();

    const char* pWithWhiteSpace = p;
    p = SkipWhiteSpace( p, encoding );

    while ( p && *p )
    {
        if ( *p != '<' )
        {
            XmlText* textNode = new XmlText( "" );

            if ( !textNode )
            {
                return 0;
            }

            if ( XmlBase::IsWhiteSpaceCondensed() )
            {
                p = textNode->Parse( p, data, encoding );
            }
            else
            {
                p = textNode->Parse( pWithWhiteSpace, data, encoding );
            }

            if ( !textNode->Blank() )
                LinkEndChild( textNode );
            else
                delete textNode;
        }
        else
        {
            if ( StringEqual( p, "</", false, encoding ) )
            {
                return p;
            }
            else
            {
                XmlNode* node = Identify( p, encoding );
                if ( node )
                {
                    p = node->Parse( p, data, encoding );
                    LinkEndChild( node );
                }
                else
                {
                    return 0;
                }
            }
        }
        pWithWhiteSpace = p;
        p = SkipWhiteSpace( p, encoding );
    }

    if ( !p )
    {
        if ( document ) document->SetError( XML_ERROR_READING_ELEMENT_VALUE, 0, 0, encoding );
    }
    return p;
}


const char* XmlUnknown::Parse( const char* p, XmlParsingData* data, XmlEncoding encoding )
{
    XmlDocument* document = GetDocument();
    p = SkipWhiteSpace( p, encoding );

    if ( data )
    {
        data->Stamp( p, encoding );
        location = data->Cursor();
    }
    if ( !p || !*p || *p != '<' )
    {
        if ( document ) document->SetError( XML_ERROR_PARSING_UNKNOWN, p, data, encoding );
        return 0;
    }
    ++p;
    value = "";

    while ( p && *p && *p != '>' )
    {
        value += *p;
        ++p;
    }

    if ( !p )
    {
        if ( document )
            document->SetError( XML_ERROR_PARSING_UNKNOWN, 0, 0, encoding );
    }
    if ( p && *p == '>' )
        return p+1;
    return p;
}


const char* XmlComment::Parse( const char* p, XmlParsingData* data, XmlEncoding encoding )
{
    XmlDocument* document = GetDocument();
    value = "";

    p = SkipWhiteSpace( p, encoding );

    if ( data )
    {
        data->Stamp( p, encoding );
        location = data->Cursor();
    }
    const char* startTag = "<!--";
    const char* endTag   = "-->";

    if ( !StringEqual( p, startTag, false, encoding ) )
    {
        if ( document )
            document->SetError( XML_ERROR_PARSING_COMMENT, p, data, encoding );
        return 0;
    }
    p += strlen( startTag );

    value = "";

    while (    p && *p && !StringEqual( p, endTag, false, encoding ) )
    {
        value.append( p, 1 );
        ++p;
    }
    if ( p && *p ) 
        p += strlen( endTag );

    return p;
}


const char* XmlAttribute::Parse( const char* p, XmlParsingData* data, XmlEncoding encoding )
{
    p = SkipWhiteSpace( p, encoding );
    if ( !p || !*p ) return 0;

    if ( data )
    {
        data->Stamp( p, encoding );
        location = data->Cursor();
    }
    
    const char* pErr = p;
    p = ReadName( p, &name, encoding );
    if ( !p || !*p )
    {
        if ( document ) document->SetError( XML_ERROR_READING_ATTRIBUTES, pErr, data, encoding );
        return 0;
    }
    p = SkipWhiteSpace( p, encoding );
    if ( !p || !*p || *p != '=' )
    {
        if ( document ) document->SetError( XML_ERROR_READING_ATTRIBUTES, p, data, encoding );
        return 0;
    }

    ++p;    
    p = SkipWhiteSpace( p, encoding );
    if ( !p || !*p )
    {
        if ( document ) document->SetError( XML_ERROR_READING_ATTRIBUTES, p, data, encoding );
        return 0;
    }
    
    const char* end;
    const char SINGLE_QUOTE = '\'';
    const char DOUBLE_QUOTE = '\"';

    if ( *p == SINGLE_QUOTE )
    {
        ++p;
        end = "\'";
        p = ReadText( p, &value, false, end, false, encoding );
    }
    else if ( *p == DOUBLE_QUOTE )
    {
        ++p;
        end = "\"";
        p = ReadText( p, &value, false, end, false, encoding );
    }
    else
    {
        value = "";
        while (    p && *p
                && !IsWhiteSpace( *p )
                && *p != '/' && *p != '>' )
        {
            if ( *p == SINGLE_QUOTE || *p == DOUBLE_QUOTE ) {
                if ( document ) document->SetError( XML_ERROR_READING_ATTRIBUTES, p, data, encoding );
                return 0;
            }
            value += *p;
            ++p;
        }
    }

    return p;
}

const char* XmlText::Parse( const char* p, XmlParsingData* data, XmlEncoding encoding )
{
    value = "";
    XmlDocument* document = GetDocument();

    if ( data )
    {
        data->Stamp( p, encoding );
        location = data->Cursor();
    }

    const char* const startTag = "<![CDATA[";
    const char* const endTag   = "]]>";

    if ( cdata || StringEqual( p, startTag, false, encoding ) )
    {
        cdata = true;

        if ( !StringEqual( p, startTag, false, encoding ) )
        {
            if ( document )
                document->SetError( XML_ERROR_PARSING_CDATA, p, data, encoding );
            return 0;
        }
        p += strlen( startTag );

        
        while (    p && *p
                && !StringEqual( p, endTag, false, encoding )
              )
        {
            value += *p;
            ++p;
        }

        XmlString dummy; 
        p = ReadText( p, &dummy, false, endTag, false, encoding );
        return p;
    }
    else
    {
        bool ignoreWhite = true;

        const char* end = "<";
        p = ReadText( p, &value, ignoreWhite, end, false, encoding );
        if ( p && *p )
            return p-1;
        return 0;
    }
}

const char* XmlDeclaration::Parse( const char* p, XmlParsingData* data, XmlEncoding _encoding )
{
    p = SkipWhiteSpace( p, _encoding );

    XmlDocument* document = GetDocument();
    if ( !p || !*p || !StringEqual( p, "<?xml", true, _encoding ) )
    {
        if ( document ) document->SetError( XML_ERROR_PARSING_DECLARATION, 0, 0, _encoding );
        return 0;
    }
    if ( data )
    {
        data->Stamp( p, _encoding );
        location = data->Cursor();
    }
    p += 5;

    version = "";
    encoding = "";
    standalone = "";

    while ( p && *p )
    {
        if ( *p == '>' )
        {
            ++p;
            return p;
        }

        p = SkipWhiteSpace( p, _encoding );
        if ( StringEqual( p, "version", true, _encoding ) )
        {
            XmlAttribute attrib;
            p = attrib.Parse( p, data, _encoding );
            version = attrib.Value();
        }
        else if ( StringEqual( p, "encoding", true, _encoding ) )
        {
            XmlAttribute attrib;
            p = attrib.Parse( p, data, _encoding );
            encoding = attrib.Value();
        }
        else if ( StringEqual( p, "standalone", true, _encoding ) )
        {
            XmlAttribute attrib;
            p = attrib.Parse( p, data, _encoding );
            standalone = attrib.Value();
        }
        else
        {
            while( p && *p && *p != '>' && !IsWhiteSpace( *p ) )
                ++p;
        }
    }

    return 0;
}

bool XmlText::Blank() const
{
    for ( unsigned i=0; i<value.length(); i++ )
        if ( !IsWhiteSpace( value[i] ) )
            return false;
    return true;
}



}