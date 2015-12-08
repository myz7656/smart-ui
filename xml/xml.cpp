#include <ctype.h>

#include "xml/xml.h"

namespace xml
{



FILE* XmlFOpen( const char* filename, const char* mode );

bool XmlBase::condenseWhiteSpace = true;


FILE* XmlFOpen( const char* filename, const char* mode )
{
    #if defined(_MSC_VER) && (_MSC_VER >= 1400 )
        FILE* fp = 0;
        errno_t err = fopen_s( &fp, filename, mode );
        if ( !err && fp )
            return fp;
        return 0;
    #else
        return fopen( filename, mode );
    #endif
}

void XmlBase::EncodeString( const XmlString& str, XmlString* outString )
{
    int i=0;

    while( i<(int)str.length() )
    {
        unsigned char c = (unsigned char) str[i];

        if (    c == '&' 
             && i < ( (int)str.length() - 2 )
             && str[i+1] == '#'
             && str[i+2] == 'x' )
        {
            while ( i<(int)str.length()-1 )
            {
                outString->append( str.c_str() + i, 1 );
                ++i;
                if ( str[i] == ';' )
                    break;
            }
        }
        else if ( c == '&' )
        {
            outString->append( entity[0].str, entity[0].strLength );
            ++i;
        }
        else if ( c == '<' )
        {
            outString->append( entity[1].str, entity[1].strLength );
            ++i;
        }
        else if ( c == '>' )
        {
            outString->append( entity[2].str, entity[2].strLength );
            ++i;
        }
        else if ( c == '\"' )
        {
            outString->append( entity[3].str, entity[3].strLength );
            ++i;
        }
        else if ( c == '\'' )
        {
            outString->append( entity[4].str, entity[4].strLength );
            ++i;
        }
        else if ( c < 32 )
        {
            char buf[ 32 ];
            
            #if defined(XML_SNPRINTF)        
                XML_SNPRINTF( buf, sizeof(buf), "&#x%02X;", (unsigned) ( c & 0xff ) );
            #else
                sprintf( buf, "&#x%02X;", (unsigned) ( c & 0xff ) );
            #endif

            outString->append( buf, (int)strlen( buf ) );
            ++i;
        }
        else
        {
            *outString += (char) c;
            ++i;
        }
    }
}


XmlNode::XmlNode( NodeType _type ) : XmlBase()
{
    parent = 0;
    type = _type;
    firstChild = 0;
    lastChild = 0;
    prev = 0;
    next = 0;
}


XmlNode::~XmlNode()
{
    XmlNode* node = firstChild;
    XmlNode* temp = 0;

    while ( node )
    {
        temp = node;
        node = node->next;
        delete temp;
    }
}


void XmlNode::CopyTo( XmlNode* target ) const
{
    target->SetValue (value.c_str() );
    target->userData = userData; 
    target->location = location;
}


void XmlNode::Clear()
{
    XmlNode* node = firstChild;
    XmlNode* temp = 0;

    while ( node )
    {
        temp = node;
        node = node->next;
        delete temp;
    }

    firstChild = 0;
    lastChild = 0;
}


XmlNode* XmlNode::LinkEndChild( XmlNode* node )
{
    assert( node->parent == 0 || node->parent == this );
    assert( node->GetDocument() == 0 || node->GetDocument() == this->GetDocument() );

    if ( node->Type() == XmlNode::XML_DOCUMENT )
    {
        delete node;
        if ( GetDocument() ) 
            GetDocument()->SetError( XML_ERROR_DOCUMENT_TOP_ONLY, 0, 0, XML_ENCODING_UNKNOWN );
        return 0;
    }

    node->parent = this;

    node->prev = lastChild;
    node->next = 0;

    if ( lastChild )
        lastChild->next = node;
    else
        firstChild = node;

    lastChild = node;
    return node;
}


XmlNode* XmlNode::InsertEndChild( const XmlNode& addThis )
{
    if ( addThis.Type() == XmlNode::XML_DOCUMENT )
    {
        if ( GetDocument() ) 
            GetDocument()->SetError( XML_ERROR_DOCUMENT_TOP_ONLY, 0, 0, XML_ENCODING_UNKNOWN );
        return 0;
    }
    XmlNode* node = addThis.Clone();
    if ( !node )
        return 0;

    return LinkEndChild( node );
}


XmlNode* XmlNode::InsertBeforeChild( XmlNode* beforeThis, const XmlNode& addThis )
{
    if ( !beforeThis || beforeThis->parent != this ) {
        return 0;
    }
    if ( addThis.Type() == XmlNode::XML_DOCUMENT )
    {
        if ( GetDocument() ) 
            GetDocument()->SetError( XML_ERROR_DOCUMENT_TOP_ONLY, 0, 0, XML_ENCODING_UNKNOWN );
        return 0;
    }

    XmlNode* node = addThis.Clone();
    if ( !node )
        return 0;
    node->parent = this;

    node->next = beforeThis;
    node->prev = beforeThis->prev;
    if ( beforeThis->prev )
    {
        beforeThis->prev->next = node;
    }
    else
    {
        assert( firstChild == beforeThis );
        firstChild = node;
    }
    beforeThis->prev = node;
    return node;
}


XmlNode* XmlNode::InsertAfterChild( XmlNode* afterThis, const XmlNode& addThis )
{
    if ( !afterThis || afterThis->parent != this ) {
        return 0;
    }
    if ( addThis.Type() == XmlNode::XML_DOCUMENT )
    {
        if ( GetDocument() ) 
            GetDocument()->SetError( XML_ERROR_DOCUMENT_TOP_ONLY, 0, 0, XML_ENCODING_UNKNOWN );
        return 0;
    }

    XmlNode* node = addThis.Clone();
    if ( !node )
        return 0;
    node->parent = this;

    node->prev = afterThis;
    node->next = afterThis->next;
    if ( afterThis->next )
    {
        afterThis->next->prev = node;
    }
    else
    {
        assert( lastChild == afterThis );
        lastChild = node;
    }
    afterThis->next = node;
    return node;
}


XmlNode* XmlNode::ReplaceChild( XmlNode* replaceThis, const XmlNode& withThis )
{
    if ( !replaceThis )
        return 0;

    if ( replaceThis->parent != this )
        return 0;

    if ( withThis.ToDocument() ) {
        
        XmlDocument* document = GetDocument();
        if ( document ) 
            document->SetError( XML_ERROR_DOCUMENT_TOP_ONLY, 0, 0, XML_ENCODING_UNKNOWN );
        return 0;
    }

    XmlNode* node = withThis.Clone();
    if ( !node )
        return 0;

    node->next = replaceThis->next;
    node->prev = replaceThis->prev;

    if ( replaceThis->next )
        replaceThis->next->prev = node;
    else
        lastChild = node;

    if ( replaceThis->prev )
        replaceThis->prev->next = node;
    else
        firstChild = node;

    delete replaceThis;
    node->parent = this;
    return node;
}


bool XmlNode::RemoveChild( XmlNode* removeThis )
{
    if ( !removeThis ) {
        return false;
    }

    if ( removeThis->parent != this )
    {
        assert( 0 );
        return false;
    }

    if ( removeThis->next )
        removeThis->next->prev = removeThis->prev;
    else
        lastChild = removeThis->prev;

    if ( removeThis->prev )
        removeThis->prev->next = removeThis->next;
    else
        firstChild = removeThis->next;

    delete removeThis;
    return true;
}

const XmlNode* XmlNode::FirstChild( const char * _value ) const
{
    const XmlNode* node;
    for ( node = firstChild; node; node = node->next )
    {
        if ( strcmp( node->Value(), _value ) == 0 )
            return node;
    }
    return 0;
}


const XmlNode* XmlNode::LastChild( const char * _value ) const
{
    const XmlNode* node;
    for ( node = lastChild; node; node = node->prev )
    {
        if ( strcmp( node->Value(), _value ) == 0 )
            return node;
    }
    return 0;
}


const XmlNode* XmlNode::IterateChildren( const XmlNode* previous ) const
{
    if ( !previous )
    {
        return FirstChild();
    }
    else
    {
        assert( previous->parent == this );
        return previous->NextSibling();
    }
}


const XmlNode* XmlNode::IterateChildren( const char * val, const XmlNode* previous ) const
{
    if ( !previous )
    {
        return FirstChild( val );
    }
    else
    {
        assert( previous->parent == this );
        return previous->NextSibling( val );
    }
}


const XmlNode* XmlNode::NextSibling( const char * _value ) const 
{
    const XmlNode* node;
    for ( node = next; node; node = node->next )
    {
        if ( strcmp( node->Value(), _value ) == 0 )
            return node;
    }
    return 0;
}


const XmlNode* XmlNode::PreviousSibling( const char * _value ) const
{
    const XmlNode* node;
    for ( node = prev; node; node = node->prev )
    {
        if ( strcmp( node->Value(), _value ) == 0 )
            return node;
    }
    return 0;
}


void XmlElement::RemoveAttribute( const char * name )
{
    XmlAttribute* node = attributeSet.Find( name );
    if ( node )
    {
        attributeSet.Remove( node );
        delete node;
    }
}

const XmlElement* XmlNode::FirstChildElement() const
{
    const XmlNode* node;

    for ( node = FirstChild(); node; node = node->NextSibling() )
    {
        if ( node->ToElement() )
            return node->ToElement();
    }
    return 0;
}


const XmlElement* XmlNode::FirstChildElement( const char * _value ) const
{
    const XmlNode* node;

    for ( node = FirstChild( _value ); node; node = node->NextSibling( _value ) )
    {
        if ( node->ToElement() )
            return node->ToElement();
    }
    return 0;
}


const XmlElement* XmlNode::NextSiblingElement() const
{
    const XmlNode* node;

    for ( node = NextSibling(); node; node = node->NextSibling() )
    {
        if ( node->ToElement() )
            return node->ToElement();
    }
    return 0;
}


const XmlElement* XmlNode::NextSiblingElement( const char * _value ) const
{
    const XmlNode* node;

    for ( node = NextSibling( _value ); node; node = node->NextSibling( _value ) )
    {
        if ( node->ToElement() )
            return node->ToElement();
    }
    return 0;
}


const XmlDocument* XmlNode::GetDocument() const
{
    const XmlNode* node;

    for( node = this; node; node = node->parent )
    {
        if ( node->ToDocument() )
            return node->ToDocument();
    }
    return 0;
}


XmlElement::XmlElement (const char * _value)
    : XmlNode( XmlNode::XML_ELEMENT )
{
    firstChild = lastChild = 0;
    value = _value;
}


XmlElement::XmlElement( const XmlElement& copy)
    : XmlNode( XmlNode::XML_ELEMENT )
{
    firstChild = lastChild = 0;
    copy.CopyTo( this );
}


XmlElement& XmlElement::operator=( const XmlElement& base )
{
    ClearThis();
    base.CopyTo( this );
    return *this;
}


XmlElement::~XmlElement()
{
    ClearThis();
}


void XmlElement::ClearThis()
{
    Clear();
    while( attributeSet.First() )
    {
        XmlAttribute* node = attributeSet.First();
        attributeSet.Remove( node );
        delete node;
    }
}


const char* XmlElement::Attribute( const char* name ) const
{
    const XmlAttribute* node = attributeSet.Find( name );
    if ( node )
        return node->Value();
    return 0;
}


const char* XmlElement::Attribute( const char* name, int* i ) const
{
    const XmlAttribute* attrib = attributeSet.Find( name );
    const char* result = 0;

    if ( attrib ) {
        result = attrib->Value();
        if ( i ) {
            attrib->QueryIntValue( i );
        }
    }
    return result;
}


const char* XmlElement::Attribute( const char* name, double* d ) const
{
    const XmlAttribute* attrib = attributeSet.Find( name );
    const char* result = 0;

    if ( attrib ) {
        result = attrib->Value();
        if ( d ) {
            attrib->QueryDoubleValue( d );
        }
    }
    return result;
}


int XmlElement::QueryIntAttribute( const char* name, int* ival ) const
{
    const XmlAttribute* attrib = attributeSet.Find( name );
    if ( !attrib )
        return XML_NO_ATTRIBUTE;
    return attrib->QueryIntValue( ival );
}


int XmlElement::QueryUnsignedAttribute( const char* name, unsigned* value ) const
{
    const XmlAttribute* node = attributeSet.Find( name );
    if ( !node )
        return XML_NO_ATTRIBUTE;

    int ival = 0;
    int result = node->QueryIntValue( &ival );
    *value = (unsigned)ival;
    return result;
}


int XmlElement::QueryBoolAttribute( const char* name, bool* bval ) const
{
    const XmlAttribute* node = attributeSet.Find( name );
    if ( !node )
        return XML_NO_ATTRIBUTE;
    
    int result = XML_WRONG_TYPE;
    if (    StringEqual( node->Value(), "true", true, XML_ENCODING_UNKNOWN ) 
         || StringEqual( node->Value(), "yes", true, XML_ENCODING_UNKNOWN ) 
         || StringEqual( node->Value(), "1", true, XML_ENCODING_UNKNOWN ) ) 
    {
        *bval = true;
        result = XML_SUCCESS;
    }
    else if (    StringEqual( node->Value(), "false", true, XML_ENCODING_UNKNOWN ) 
              || StringEqual( node->Value(), "no", true, XML_ENCODING_UNKNOWN ) 
              || StringEqual( node->Value(), "0", true, XML_ENCODING_UNKNOWN ) ) 
    {
        *bval = false;
        result = XML_SUCCESS;
    }
    return result;
}


int XmlElement::QueryDoubleAttribute( const char* name, double* dval ) const
{
    const XmlAttribute* attrib = attributeSet.Find( name );
    if ( !attrib )
        return XML_NO_ATTRIBUTE;
    return attrib->QueryDoubleValue( dval );
}


void XmlElement::SetAttribute( const char * name, int val )
{
    XmlAttribute* attrib = attributeSet.FindOrCreate( name );
    if ( attrib ) {
        attrib->SetIntValue( val );
    }
}


void XmlElement::SetDoubleAttribute( const char * name, double val )
{
    XmlAttribute* attrib = attributeSet.FindOrCreate( name );
    if ( attrib ) {
        attrib->SetDoubleValue( val );
    }
}


void XmlElement::SetAttribute( const char * cname, const char * cvalue )
{
    XmlAttribute* attrib = attributeSet.FindOrCreate( cname );
    if ( attrib ) {
        attrib->SetValue( cvalue );
    }
}


void XmlElement::Print( FILE* cfile, int depth ) const
{
    int i;
    assert( cfile );
    for ( i=0; i<depth; i++ ) {
        fprintf( cfile, "    " );
    }

    fprintf( cfile, "<%s", value.c_str() );

    const XmlAttribute* attrib;
    for ( attrib = attributeSet.First(); attrib; attrib = attrib->Next() )
    {
        fprintf( cfile, " " );
        attrib->Print( cfile, depth );
    }

    XmlNode* node;
    if ( !firstChild )
    {
        fprintf( cfile, " />" );
    }
    else if ( firstChild == lastChild && firstChild->ToText() )
    {
        fprintf( cfile, ">" );
        firstChild->Print( cfile, depth + 1 );
        fprintf( cfile, "</%s>", value.c_str() );
    }
    else
    {
        fprintf( cfile, ">" );

        for ( node = firstChild; node; node=node->NextSibling() )
        {
            if ( !node->ToText() )
            {
                fprintf( cfile, "\n" );
            }
            node->Print( cfile, depth+1 );
        }
        fprintf( cfile, "\n" );
        for( i=0; i<depth; ++i ) {
            fprintf( cfile, "    " );
        }
        fprintf( cfile, "</%s>", value.c_str() );
    }
}


void XmlElement::CopyTo( XmlElement* target ) const
{
    XmlNode::CopyTo( target );

    const XmlAttribute* attribute = 0;
    for( attribute = attributeSet.First(); attribute; attribute = attribute->Next() )
    {
        target->SetAttribute( attribute->Name(), attribute->Value() );
    }

    XmlNode* node = 0;
    for ( node = firstChild; node; node = node->NextSibling() )
    {
        target->LinkEndChild( node->Clone() );
    }
}

bool XmlElement::Accept( XmlVisitor* visitor ) const
{
    if ( visitor->VisitEnter( *this, attributeSet.First() ) ) 
    {
        for ( const XmlNode* node=FirstChild(); node; node=node->NextSibling() )
        {
            if ( !node->Accept( visitor ) )
                break;
        }
    }
    return visitor->VisitExit( *this );
}


XmlNode* XmlElement::Clone() const
{
    XmlElement* clone = new XmlElement( Value() );
    if ( !clone )
        return 0;

    CopyTo( clone );
    return clone;
}


const char* XmlElement::GetText() const
{
    const XmlNode* child = this->FirstChild();
    if ( child ) {
        const XmlText* childText = child->ToText();
        if ( childText ) {
            return childText->Value();
        }
    }
    return 0;
}


XmlDocument::XmlDocument() : XmlNode( XmlNode::XML_DOCUMENT )
{
    tabsize = 4;
    useMicrosoftBOM = false;
    ClearError();
}

XmlDocument::XmlDocument( const char * documentName ) : XmlNode( XmlNode::XML_DOCUMENT )
{
    tabsize = 4;
    useMicrosoftBOM = false;
    value = documentName;
    ClearError();
}


XmlDocument::XmlDocument( const XmlDocument& copy ) : XmlNode( XmlNode::XML_DOCUMENT )
{
    copy.CopyTo( this );
}


XmlDocument& XmlDocument::operator=( const XmlDocument& copy )
{
    Clear();
    copy.CopyTo( this );
    return *this;
}


bool XmlDocument::LoadFile( XmlEncoding encoding )
{
    return LoadFile( Value(), encoding );
}


bool XmlDocument::SaveFile() const
{
    return SaveFile( Value() );
}

bool XmlDocument::LoadFile( const char* _filename, XmlEncoding encoding )
{
    XmlString filename( _filename );
    value = filename;

    FILE* file = XmlFOpen( value.c_str (), "rb" );

    if ( file )
    {
        bool result = LoadFile( file, encoding );
        fclose( file );
        return result;
    }
    else
    {
        SetError( XML_ERROR_OPENING_FILE, 0, 0, XML_ENCODING_UNKNOWN );
        return false;
    }
}

bool XmlDocument::LoadFile( FILE* file, XmlEncoding encoding )
{
    if ( !file ) 
    {
        SetError( XML_ERROR_OPENING_FILE, 0, 0, XML_ENCODING_UNKNOWN );
        return false;
    }

    Clear();
    location.Clear();

    long length = 0;
    fseek( file, 0, SEEK_END );
    length = ftell( file );
    fseek( file, 0, SEEK_SET );

    if ( length <= 0 )
    {
        SetError( XML_ERROR_DOCUMENT_EMPTY, 0, 0, XML_ENCODING_UNKNOWN );
        return false;
    }

    char* buf = new char[ length+1 ];
    buf[0] = 0;

    if ( fread( buf, length, 1, file ) != 1 ) {
        delete [] buf;
        SetError( XML_ERROR_OPENING_FILE, 0, 0, XML_ENCODING_UNKNOWN );
        return false;
    }

    const char* p = buf;
    char* q = buf;
    const char CR = 0x0d;
    const char LF = 0x0a;

    buf[length] = 0;
    while( *p ) {
        assert( p < (buf+length) );
        assert( q <= (buf+length) );
        assert( q <= p );

        if ( *p == CR ) {
            *q++ = LF;
            p++;
            if ( *p == LF ) {
                p++;
            }
        }
        else {
            *q++ = *p++;
        }
    }
    assert( q <= (buf+length) );
    *q = 0;

    Parse( buf, 0, encoding );

    delete [] buf;
    return !Error();
}


bool XmlDocument::SaveFile( const char * filename ) const
{
    FILE* fp = XmlFOpen( filename, "w" );
    if ( fp )
    {
        bool result = SaveFile( fp );
        fclose( fp );
        return result;
    }
    return false;
}


bool XmlDocument::SaveFile( FILE* fp ) const
{
    if ( useMicrosoftBOM ) 
    {
        const unsigned char XML_UTF_LEAD_0 = 0xefU;
        const unsigned char XML_UTF_LEAD_1 = 0xbbU;
        const unsigned char XML_UTF_LEAD_2 = 0xbfU;

        fputc( XML_UTF_LEAD_0, fp );
        fputc( XML_UTF_LEAD_1, fp );
        fputc( XML_UTF_LEAD_2, fp );
    }
    Print( fp, 0 );
    return (ferror(fp) == 0);
}


void XmlDocument::CopyTo( XmlDocument* target ) const
{
    XmlNode::CopyTo( target );

    target->error = error;
    target->errorId = errorId;
    target->errorDesc = errorDesc;
    target->tabsize = tabsize;
    target->errorLocation = errorLocation;
    target->useMicrosoftBOM = useMicrosoftBOM;

    XmlNode* node = 0;
    for ( node = firstChild; node; node = node->NextSibling() )
    {
        target->LinkEndChild( node->Clone() );
    }
}


XmlNode* XmlDocument::Clone() const
{
    XmlDocument* clone = new XmlDocument();
    if ( !clone )
        return 0;

    CopyTo( clone );
    return clone;
}


void XmlDocument::Print( FILE* cfile, int depth ) const
{
    assert( cfile );
    for ( const XmlNode* node=FirstChild(); node; node=node->NextSibling() )
    {
        node->Print( cfile, depth );
        fprintf( cfile, "\n" );
    }
}


bool XmlDocument::Accept( XmlVisitor* visitor ) const
{
    if ( visitor->VisitEnter( *this ) )
    {
        for ( const XmlNode* node=FirstChild(); node; node=node->NextSibling() )
        {
            if ( !node->Accept( visitor ) )
                break;
        }
    }
    return visitor->VisitExit( *this );
}


const XmlAttribute* XmlAttribute::Next() const
{
    if ( next->value.empty() && next->name.empty() )
        return 0;
    return next;
}

const XmlAttribute* XmlAttribute::Previous() const
{
    if ( prev->value.empty() && prev->name.empty() )
        return 0;
    return prev;
}

void XmlAttribute::Print( FILE* cfile, int, XmlString* str ) const
{
    XmlString n, v;

    EncodeString( name, &n );
    EncodeString( value, &v );

    if (value.find ('\"') == XmlString::npos) {
        if ( cfile ) {
            fprintf (cfile, "%s=\"%s\"", n.c_str(), v.c_str() );
        }
        if ( str ) {
            (*str) += n; (*str) += "=\""; (*str) += v; (*str) += "\"";
        }
    }
    else {
        if ( cfile ) {
            fprintf (cfile, "%s='%s'", n.c_str(), v.c_str() );
        }
        if ( str ) {
            (*str) += n; (*str) += "='"; (*str) += v; (*str) += "'";
        }
    }
}


int XmlAttribute::QueryIntValue( int* ival ) const
{
    if ( XML_SSCANF( value.c_str(), "%d", ival ) == 1 )
        return XML_SUCCESS;
    return XML_WRONG_TYPE;
}

int XmlAttribute::QueryDoubleValue( double* dval ) const
{
    if ( XML_SSCANF( value.c_str(), "%lf", dval ) == 1 )
        return XML_SUCCESS;
    return XML_WRONG_TYPE;
}

void XmlAttribute::SetIntValue( int _value )
{
    char buf [64];
    #if defined(XML_SNPRINTF)        
        XML_SNPRINTF(buf, sizeof(buf), "%d", _value);
    #else
        sprintf (buf, "%d", _value);
    #endif
    SetValue (buf);
}

void XmlAttribute::SetDoubleValue( double _value )
{
    char buf [256];
    #if defined(XML_SNPRINTF)        
        XML_SNPRINTF( buf, sizeof(buf), "%g", _value);
    #else
        sprintf (buf, "%g", _value);
    #endif
    SetValue (buf);
}

int XmlAttribute::IntValue() const
{
    return atoi (value.c_str ());
}

double  XmlAttribute::DoubleValue() const
{
    return atof (value.c_str ());
}


XmlComment::XmlComment( const XmlComment& copy ) : XmlNode( XmlNode::XML_COMMENT )
{
    copy.CopyTo( this );
}


XmlComment& XmlComment::operator=( const XmlComment& base )
{
    Clear();
    base.CopyTo( this );
    return *this;
}


void XmlComment::Print( FILE* cfile, int depth ) const
{
    assert( cfile );
    for ( int i=0; i<depth; i++ )
    {
        fprintf( cfile,  "    " );
    }
    fprintf( cfile, "<!--%s-->", value.c_str() );
}


void XmlComment::CopyTo( XmlComment* target ) const
{
    XmlNode::CopyTo( target );
}


bool XmlComment::Accept( XmlVisitor* visitor ) const
{
    return visitor->Visit( *this );
}


XmlNode* XmlComment::Clone() const
{
    XmlComment* clone = new XmlComment();

    if ( !clone )
        return 0;

    CopyTo( clone );
    return clone;
}


void XmlText::Print( FILE* cfile, int depth ) const
{
    assert( cfile );
    if ( cdata )
    {
        int i;
        fprintf( cfile, "\n" );
        for ( i=0; i<depth; i++ ) {
            fprintf( cfile, "    " );
        }
        fprintf( cfile, "<![CDATA[%s]]>\n", value.c_str() );    
    }
    else
    {
        XmlString buffer;
        EncodeString( value, &buffer );
        fprintf( cfile, "%s", buffer.c_str() );
    }
}


void XmlText::CopyTo( XmlText* target ) const
{
    XmlNode::CopyTo( target );
    target->cdata = cdata;
}


bool XmlText::Accept( XmlVisitor* visitor ) const
{
    return visitor->Visit( *this );
}


XmlNode* XmlText::Clone() const
{    
    XmlText* clone = 0;
    clone = new XmlText( "" );

    if ( !clone )
        return 0;

    CopyTo( clone );
    return clone;
}


XmlDeclaration::XmlDeclaration( const char * _version,
                                    const char * _encoding,
                                    const char * _standalone )
    : XmlNode( XmlNode::XML_DECLARATION )
{
    version = _version;
    encoding = _encoding;
    standalone = _standalone;
}


XmlDeclaration::XmlDeclaration( const XmlDeclaration& copy )
    : XmlNode( XmlNode::XML_DECLARATION )
{
    copy.CopyTo( this );
}


XmlDeclaration& XmlDeclaration::operator=( const XmlDeclaration& copy )
{
    Clear();
    copy.CopyTo( this );
    return *this;
}


void XmlDeclaration::Print( FILE* cfile, int, XmlString* str ) const
{
    if ( cfile ) fprintf( cfile, "<?xml " );
    if ( str )     (*str) += "<?xml ";

    if ( !version.empty() ) {
        if ( cfile ) fprintf (cfile, "version=\"%s\" ", version.c_str ());
        if ( str ) { (*str) += "version=\""; (*str) += version; (*str) += "\" "; }
    }
    if ( !encoding.empty() ) {
        if ( cfile ) fprintf (cfile, "encoding=\"%s\" ", encoding.c_str ());
        if ( str ) { (*str) += "encoding=\""; (*str) += encoding; (*str) += "\" "; }
    }
    if ( !standalone.empty() ) {
        if ( cfile ) fprintf (cfile, "standalone=\"%s\" ", standalone.c_str ());
        if ( str ) { (*str) += "standalone=\""; (*str) += standalone; (*str) += "\" "; }
    }
    if ( cfile ) fprintf( cfile, "?>" );
    if ( str )     (*str) += "?>";
}


void XmlDeclaration::CopyTo( XmlDeclaration* target ) const
{
    XmlNode::CopyTo( target );

    target->version = version;
    target->encoding = encoding;
    target->standalone = standalone;
}


bool XmlDeclaration::Accept( XmlVisitor* visitor ) const
{
    return visitor->Visit( *this );
}


XmlNode* XmlDeclaration::Clone() const
{    
    XmlDeclaration* clone = new XmlDeclaration();

    if ( !clone )
        return 0;

    CopyTo( clone );
    return clone;
}


void XmlUnknown::Print( FILE* cfile, int depth ) const
{
    for ( int i=0; i<depth; i++ )
        fprintf( cfile, "    " );
    fprintf( cfile, "<%s>", value.c_str() );
}


void XmlUnknown::CopyTo( XmlUnknown* target ) const
{
    XmlNode::CopyTo( target );
}


bool XmlUnknown::Accept( XmlVisitor* visitor ) const
{
    return visitor->Visit( *this );
}


XmlNode* XmlUnknown::Clone() const
{
    XmlUnknown* clone = new XmlUnknown();

    if ( !clone )
        return 0;

    CopyTo( clone );
    return clone;
}


XmlAttributeSet::XmlAttributeSet()
{
    sentinel.next = &sentinel;
    sentinel.prev = &sentinel;
}


XmlAttributeSet::~XmlAttributeSet()
{
    assert( sentinel.next == &sentinel );
    assert( sentinel.prev == &sentinel );
}


void XmlAttributeSet::Add( XmlAttribute* addMe )
{
    assert( !Find( addMe->Name() ) );

    addMe->next = &sentinel;
    addMe->prev = sentinel.prev;

    sentinel.prev->next = addMe;
    sentinel.prev      = addMe;
}

void XmlAttributeSet::Remove( XmlAttribute* removeMe )
{
    XmlAttribute* node;

    for( node = sentinel.next; node != &sentinel; node = node->next )
    {
        if ( node == removeMe )
        {
            node->prev->next = node->next;
            node->next->prev = node->prev;
            node->next = 0;
            node->prev = 0;
            return;
        }
    }
    assert( 0 );
}


XmlAttribute* XmlAttributeSet::Find( const char* name ) const
{
    for( XmlAttribute* node = sentinel.next; node != &sentinel; node = node->next )
    {
        if ( strcmp( node->name.c_str(), name ) == 0 )
            return node;
    }
    return 0;
}


XmlAttribute* XmlAttributeSet::FindOrCreate( const char* _name )
{
    XmlAttribute* attrib = Find( _name );
    if ( !attrib ) {
        attrib = new XmlAttribute();
        Add( attrib );
        attrib->SetName( _name );
    }
    return attrib;
}


XmlHandle XmlHandle::FirstChild() const
{
    if ( node )
    {
        XmlNode* child = node->FirstChild();
        if ( child )
            return XmlHandle( child );
    }
    return XmlHandle( 0 );
}


XmlHandle XmlHandle::FirstChild( const char * value ) const
{
    if ( node )
    {
        XmlNode* child = node->FirstChild( value );
        if ( child )
            return XmlHandle( child );
    }
    return XmlHandle( 0 );
}


XmlHandle XmlHandle::FirstChildElement() const
{
    if ( node )
    {
        XmlElement* child = node->FirstChildElement();
        if ( child )
            return XmlHandle( child );
    }
    return XmlHandle( 0 );
}


XmlHandle XmlHandle::FirstChildElement( const char * value ) const
{
    if ( node )
    {
        XmlElement* child = node->FirstChildElement( value );
        if ( child )
            return XmlHandle( child );
    }
    return XmlHandle( 0 );
}


XmlHandle XmlHandle::Child( int count ) const
{
    if ( node )
    {
        int i;
        XmlNode* child = node->FirstChild();
        for (    i=0;
                child && i<count;
                child = child->NextSibling(), ++i )
        {

        }
        if ( child )
            return XmlHandle( child );
    }
    return XmlHandle( 0 );
}


XmlHandle XmlHandle::Child( const char* value, int count ) const
{
    if ( node )
    {
        int i;
        XmlNode* child = node->FirstChild( value );
        for (    i=0;
                child && i<count;
                child = child->NextSibling( value ), ++i )
        {

        }
        if ( child )
            return XmlHandle( child );
    }
    return XmlHandle( 0 );
}


XmlHandle XmlHandle::ChildElement( int count ) const
{
    if ( node )
    {
        int i;
        XmlElement* child = node->FirstChildElement();
        for (    i=0;
                child && i<count;
                child = child->NextSiblingElement(), ++i )
        {

        }
        if ( child )
            return XmlHandle( child );
    }
    return XmlHandle( 0 );
}


XmlHandle XmlHandle::ChildElement( const char* value, int count ) const
{
    if ( node )
    {
        int i;
        XmlElement* child = node->FirstChildElement( value );
        for (    i=0;
                child && i<count;
                child = child->NextSiblingElement( value ), ++i )
        {

        }
        if ( child )
            return XmlHandle( child );
    }
    return XmlHandle( 0 );
}


bool XmlPrinter::VisitEnter( const XmlDocument& )
{
    return true;
}

bool XmlPrinter::VisitExit( const XmlDocument& )
{
    return true;
}

bool XmlPrinter::VisitEnter( const XmlElement& element, const XmlAttribute* firstAttribute )
{
    DoIndent();
    buffer += "<";
    buffer += element.Value();

    for( const XmlAttribute* attrib = firstAttribute; attrib; attrib = attrib->Next() )
    {
        buffer += " ";
        attrib->Print( 0, 0, &buffer );
    }

    if ( !element.FirstChild() ) 
    {
        buffer += " />";
        DoLineBreak();
    }
    else
    {
        buffer += ">";
        if (    element.FirstChild()->ToText()
              && element.LastChild() == element.FirstChild()
              && element.FirstChild()->ToText()->CDATA() == false )
        {
            simpleTextPrint = true;
        }
        else
        {
            DoLineBreak();
        }
    }
    ++depth;
    return true;
}


bool XmlPrinter::VisitExit( const XmlElement& element )
{
    --depth;
    if ( !element.FirstChild() )
    {
    }
    else
    {
        if ( simpleTextPrint )
        {
            simpleTextPrint = false;
        }
        else
        {
            DoIndent();
        }
        buffer += "</";
        buffer += element.Value();
        buffer += ">";
        DoLineBreak();
    }
    return true;
}


bool XmlPrinter::Visit( const XmlText& text )
{
    if ( text.CDATA() )
    {
        DoIndent();
        buffer += "<![CDATA[";
        buffer += text.Value();
        buffer += "]]>";
        DoLineBreak();
    }
    else if ( simpleTextPrint )
    {
        XmlString str;
        XmlBase::EncodeString( text.ValueTStr(), &str );
        buffer += str;
    }
    else
    {
        DoIndent();
        XmlString str;
        XmlBase::EncodeString( text.ValueTStr(), &str );
        buffer += str;
        DoLineBreak();
    }
    return true;
}


bool XmlPrinter::Visit( const XmlDeclaration& declaration )
{
    DoIndent();
    declaration.Print( 0, 0, &buffer );
    DoLineBreak();
    return true;
}


bool XmlPrinter::Visit( const XmlComment& comment )
{
    DoIndent();
    buffer += "<!--";
    buffer += comment.Value();
    buffer += "-->";
    DoLineBreak();
    return true;
}


bool XmlPrinter::Visit( const XmlUnknown& unknown )
{
    DoIndent();
    buffer += "<";
    buffer += unknown.Value();
    buffer += ">";
    DoLineBreak();
    return true;
}



}