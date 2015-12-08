#ifndef __xml_xml_h__
#define __xml_xml_h__

#ifdef _MSC_VER
#pragma warning( push )
#pragma warning( disable : 4530 )
#pragma warning( disable : 4786 )
#endif

#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>


#if defined( _DEBUG ) && !defined( DEBUG )
#define DEBUG
#endif

#include "xml/xmlstr.h"

#define XML_SAFE

#ifdef XML_SAFE
    #if defined(_MSC_VER) && (_MSC_VER >= 1400 )
        #define XML_SNPRINTF _snprintf_s
        #define XML_SSCANF   sscanf_s
    #elif defined(_MSC_VER) && (_MSC_VER >= 1200 )
        #define XML_SNPRINTF _snprintf
        #define XML_SSCANF   sscanf
    #elif defined(__GNUC__) && (__GNUC__ >= 3 )
        #define XML_SNPRINTF snprintf
        #define XML_SSCANF   sscanf
    #else
        #define XML_SNPRINTF snprintf
        #define XML_SSCANF   sscanf
    #endif
#endif

namespace xml
{



class XmlDocument;
class XmlElement;
class XmlComment;
class XmlUnknown;
class XmlAttribute;
class XmlText;
class XmlDeclaration;
class XmlParsingData;


struct XmlCursor
{
    XmlCursor() { Clear(); }
    void Clear() { row = col = -1; }

    int row;
    int col;
};


class XmlVisitor
{
public:
    virtual ~XmlVisitor() {}

    virtual bool VisitEnter( const XmlDocument& ) { return true; }

    virtual bool VisitExit( const XmlDocument& ) { return true; }

    virtual bool VisitEnter( const XmlElement& , const XmlAttribute* ) { return true; }

    virtual bool VisitExit( const XmlElement& ) { return true; }

    virtual bool Visit( const XmlDeclaration& ) { return true; }

    virtual bool Visit( const XmlText& ) { return true; }

    virtual bool Visit( const XmlComment& ) { return true; }

    virtual bool Visit( const XmlUnknown& ) { return true; }
};


enum
{
    XML_SUCCESS,
    XML_NO_ATTRIBUTE,
    XML_WRONG_TYPE
};


enum XmlEncoding
{
    XML_ENCODING_UNKNOWN,
    XML_ENCODING_UTF8,
    XML_ENCODING_LEGACY
};

const XmlEncoding XML_DEFAULT_ENCODING = XML_ENCODING_UNKNOWN;

class XmlBase
{
    friend class XmlNode;
    friend class XmlElement;
    friend class XmlDocument;

public:
    XmlBase() : userData(0) {}
    virtual ~XmlBase() {}

    virtual void Print( FILE* cfile, int depth ) const = 0;

    static void SetCondenseWhiteSpace( bool condense ) { condenseWhiteSpace = condense; }

    static bool IsWhiteSpaceCondensed() { return condenseWhiteSpace; }

    int Row() const { return location.row + 1; }
    int Column() const { return location.col + 1; }

    void  SetUserData( void* user ) { userData = user; }
    void* GetUserData() { return userData; }
    const void* GetUserData() const { return userData; }

    static const int utf8ByteTable[256];

    virtual const char* Parse( const char* p, XmlParsingData* data, XmlEncoding encoding ) = 0;

    static void EncodeString( const XmlString& str, XmlString* out );

    enum
    {
        XML_NO_ERROR = 0,
        XML_ERROR,
        XML_ERROR_OPENING_FILE,
        XML_ERROR_PARSING_ELEMENT,
        XML_ERROR_FAILED_TO_READ_ELEMENT_NAME,
        XML_ERROR_READING_ELEMENT_VALUE,
        XML_ERROR_READING_ATTRIBUTES,
        XML_ERROR_PARSING_EMPTY,
        XML_ERROR_READING_END_TAG,
        XML_ERROR_PARSING_UNKNOWN,
        XML_ERROR_PARSING_COMMENT,
        XML_ERROR_PARSING_DECLARATION,
        XML_ERROR_DOCUMENT_EMPTY,
        XML_ERROR_EMBEDDED_NULL,
        XML_ERROR_PARSING_CDATA,
        XML_ERROR_DOCUMENT_TOP_ONLY,

        XML_ERROR_STRING_COUNT
    };

public:
    static const char* SkipWhiteSpace( const char*, XmlEncoding encoding );

    inline static bool IsWhiteSpace( char c )
    {
        return ( isspace( (unsigned char) c ) || c == '\n' || c == '\r' );
    }

    inline static bool IsWhiteSpace( int c )
    {
        if ( c < 256 )
            return IsWhiteSpace( (char) c );
        return false;
    }

    static const char* ReadName( const char* p, XmlString* name, XmlEncoding encoding );

    static const char* ReadText(    const char* in,
                                    XmlString* text,
                                    bool ignoreWhiteSpace,
                                    const char* endTag,
                                    bool ignoreCase,
                                    XmlEncoding encoding );

    static const char* ReadText( const char* in,
                                 XmlString* text,
                                 bool ignoreWhiteSpace,
                                 bool keepSingleWhiteSpace,
                                 const char* endTag,
                                 bool ignoreCase,
                                 XmlEncoding encoding );

    static const char* GetEntity( const char* in, char* value, int* length, XmlEncoding encoding );

    inline static const char* GetChar( const char* p, char* _value, int* length, XmlEncoding encoding )
    {
        assert( p );
        if ( encoding == XML_ENCODING_UTF8 )
        {
            *length = utf8ByteTable[ *((const unsigned char*)p) ];
            assert( *length >= 0 && *length < 5 );
        }
        else
        {
            *length = 1;
        }

        if ( *length == 1 )
        {
            if ( *p == '&' )
                return GetEntity( p, _value, length, encoding );
            *_value = *p;
            return p+1;
        }
        else if ( *length )
        {
            for( int i=0; p[i] && i<*length; ++i ) {
                _value[i] = p[i];
            }
            return p + (*length);
        }
        else
        {
            return 0;
        }
    }

    static bool StringEqual(    const char* p,
                                const char* endTag,
                                bool ignoreCase,
                                XmlEncoding encoding );

    static const char* errorString[ XML_ERROR_STRING_COUNT ];

    XmlCursor location;

    void* userData;

    static int IsAlpha( unsigned char anyByte, XmlEncoding encoding );
    static int IsAlphaNum( unsigned char anyByte, XmlEncoding encoding );
    inline static int ToLower( int v, XmlEncoding encoding )
    {
        if ( encoding == XML_ENCODING_UTF8 )
        {
            if ( v < 128 ) return tolower( v );
            return v;
        }
        else
        {
            return tolower( v );
        }
    }

    static void ConvertUTF32ToUTF8( unsigned long input, char* output, int* length );

private:
    XmlBase( const XmlBase& );
    void operator=( const XmlBase& base );

    struct Entity
    {
        const char*     str;
        unsigned int    strLength;
        char            chr;
    };
    enum
    {
        NUM_ENTITY = 5,
        MAX_ENTITY_LENGTH = 6
    };

    static Entity entity[ NUM_ENTITY ];
    static bool condenseWhiteSpace;
};


class XmlNode : public XmlBase
{
    friend class XmlDocument;
    friend class XmlElement;

public:
    enum NodeType
    {
        XML_DOCUMENT,
        XML_ELEMENT,
        XML_COMMENT,
        XML_UNKNOWN,
        XML_TEXT,
        XML_DECLARATION,
        XML_TYPECOUNT
    };

    virtual ~XmlNode();

    const char *Value() const { return value.c_str (); }

    const XmlString& ValueTStr() const { return value; }

    void SetValue(const char * _value) { value = _value;}

    void Clear();

    XmlNode* Parent()                            { return parent; }
    const XmlNode* Parent() const                { return parent; }

    const XmlNode* FirstChild() const            { return firstChild; }
    XmlNode* FirstChild()                        { return firstChild; }
    const XmlNode* FirstChild( const char * value ) const;

    XmlNode* FirstChild( const char * _value )
    {
        return const_cast< XmlNode* > ((const_cast< const XmlNode* >(this))->FirstChild( _value ));
    }
    const XmlNode* LastChild() const { return lastChild; }
    XmlNode* LastChild()             { return lastChild; }
    
    const XmlNode* LastChild( const char * value ) const;
    XmlNode* LastChild( const char * _value )
    {
        return const_cast< XmlNode* > ((const_cast< const XmlNode* >(this))->LastChild( _value ));
    }

    const XmlNode* IterateChildren( const XmlNode* previous ) const;
    XmlNode* IterateChildren( const XmlNode* previous )
    {
        return const_cast< XmlNode* >( (const_cast< const XmlNode* >(this))->IterateChildren( previous ) );
    }

    const XmlNode* IterateChildren( const char * value, const XmlNode* previous ) const;
    XmlNode* IterateChildren( const char * _value, const XmlNode* previous )
    {
        return const_cast< XmlNode* >( (const_cast< const XmlNode* >(this))->IterateChildren( _value, previous ) );
    }

    XmlNode* InsertEndChild( const XmlNode& addThis );

    XmlNode* LinkEndChild( XmlNode* addThis );

    XmlNode* InsertBeforeChild( XmlNode* beforeThis, const XmlNode& addThis );

    XmlNode* InsertAfterChild(  XmlNode* afterThis, const XmlNode& addThis );

    XmlNode* ReplaceChild( XmlNode* replaceThis, const XmlNode& withThis );

    bool RemoveChild( XmlNode* removeThis );

    const XmlNode* PreviousSibling() const            { return prev; }
    XmlNode* PreviousSibling()                        { return prev; }

    const XmlNode* PreviousSibling( const char * ) const;
    XmlNode* PreviousSibling( const char *_prev )
    {
        return const_cast< XmlNode* >( (const_cast< const XmlNode* >(this))->PreviousSibling( _prev ) );
    }

    const XmlNode* NextSibling() const { return next; }
    XmlNode* NextSibling()             { return next; }

    const XmlNode* NextSibling( const char * ) const;
    XmlNode* NextSibling( const char* _next )
    {
        return const_cast< XmlNode* >( (const_cast< const XmlNode* >(this))->NextSibling( _next ) );
    }

    const XmlElement* NextSiblingElement() const;
    XmlElement* NextSiblingElement()
    {
        return const_cast< XmlElement* >( (const_cast< const XmlNode* >(this))->NextSiblingElement() );
    }

    const XmlElement* NextSiblingElement( const char * ) const;
    XmlElement* NextSiblingElement( const char *_next )
    {
        return const_cast< XmlElement* >( (const_cast< const XmlNode* >(this))->NextSiblingElement( _next ) );
    }

    const XmlElement* FirstChildElement() const;
    XmlElement* FirstChildElement()
    {
        return const_cast< XmlElement* >( (const_cast< const XmlNode* >(this))->FirstChildElement() );
    }

    const XmlElement* FirstChildElement( const char * _value ) const;
    XmlElement* FirstChildElement( const char * _value )
    {
        return const_cast< XmlElement* >( (const_cast< const XmlNode* >(this))->FirstChildElement( _value ) );
    }

    int Type() const    { return type; }

    const XmlDocument* GetDocument() const;
    XmlDocument* GetDocument()
    {
        return const_cast< XmlDocument* >( (const_cast< const XmlNode* >(this))->GetDocument() );
    }

    bool NoChildren() const { return !firstChild; }

    virtual const XmlDocument*    ToDocument()    const { return 0; }
    virtual const XmlElement*     ToElement()     const { return 0; }
    virtual const XmlComment*     ToComment()     const { return 0; }
    virtual const XmlUnknown*     ToUnknown()     const { return 0; }
    virtual const XmlText*        ToText()        const { return 0; }
    virtual const XmlDeclaration* ToDeclaration() const { return 0; }

    virtual XmlDocument*          ToDocument()    { return 0; }
    virtual XmlElement*           ToElement()     { return 0; }
    virtual XmlComment*           ToComment()     { return 0; }
    virtual XmlUnknown*           ToUnknown()     { return 0; }
    virtual XmlText*              ToText()        { return 0; }
    virtual XmlDeclaration*       ToDeclaration() { return 0; }

    virtual XmlNode* Clone() const = 0;

    virtual bool Accept( XmlVisitor* visitor ) const = 0;

protected:
    XmlNode( NodeType _type );

    void CopyTo( XmlNode* target ) const;

    XmlNode* Identify( const char* start, XmlEncoding encoding );

    XmlNode*        parent;
    NodeType        type;

    XmlNode*        firstChild;
    XmlNode*        lastChild;

    XmlString       value;

    XmlNode*        prev;
    XmlNode*        next;

private:
    XmlNode( const XmlNode& );
    void operator=( const XmlNode& base );
};

class XmlAttribute : public XmlBase
{
    friend class XmlAttributeSet;

public:
    XmlAttribute() : XmlBase()
    {
        document = 0;
        prev = next = 0;
    }

    XmlAttribute( const char * _name, const char * _value )
    {
        name = _name;
        value = _value;
        document = 0;
        prev = next = 0;
    }

    const char*        Name()  const { return name.c_str(); }
    const char*        Value() const { return value.c_str(); }

    int                IntValue() const;
    double             DoubleValue() const;

    const XmlString& NameTStr() const { return name; }

    int QueryIntValue( int* _value ) const;
    
    int QueryDoubleValue( double* _value ) const;

    void SetName( const char* _name ) { name = _name; }
    void SetValue( const char* _value ) { value = _value; }

    void SetIntValue( int _value );
    void SetDoubleValue( double _value );

    const XmlAttribute* Next() const;
    XmlAttribute* Next()
    {
        return const_cast< XmlAttribute* >( (const_cast< const XmlAttribute* >(this))->Next() );
    }

    const XmlAttribute* Previous() const;
    XmlAttribute* Previous()
    {
        return const_cast< XmlAttribute* >( (const_cast< const XmlAttribute* >(this))->Previous() ); 
    }

    bool operator==( const XmlAttribute& rhs ) const { return rhs.name == name; }
    bool operator<( const XmlAttribute& rhs )     const { return name < rhs.name; }
    bool operator>( const XmlAttribute& rhs )  const { return name > rhs.name; }

    virtual const char* Parse( const char* p, XmlParsingData* data, XmlEncoding encoding );

    virtual void Print( FILE* cfile, int depth ) const
    {
        Print( cfile, depth, 0 );
    }
    void Print( FILE* cfile, int depth, XmlString* str ) const;

    void SetDocument( XmlDocument* doc ) { document = doc; }

private:
    XmlAttribute( const XmlAttribute& );
    void operator=( const XmlAttribute& base );

    XmlDocument*    document;
    XmlString       name;
    XmlString       value;
    XmlAttribute*   prev;
    XmlAttribute*   next;
};

class XmlAttributeSet
{
public:
    XmlAttributeSet();
    ~XmlAttributeSet();

    void Add( XmlAttribute* attribute );
    void Remove( XmlAttribute* attribute );

    const XmlAttribute* First() const       { return ( sentinel.next == &sentinel ) ? 0 : sentinel.next; }
    XmlAttribute* First()                   { return ( sentinel.next == &sentinel ) ? 0 : sentinel.next; }
    const XmlAttribute* Last() const        { return ( sentinel.prev == &sentinel ) ? 0 : sentinel.prev; }
    XmlAttribute* Last()                    { return ( sentinel.prev == &sentinel ) ? 0 : sentinel.prev; }

    XmlAttribute*    Find( const char* _name ) const;
    XmlAttribute* FindOrCreate( const char* _name );

private:
    XmlAttributeSet( const XmlAttributeSet& );
    void operator=( const XmlAttributeSet& );

    XmlAttribute sentinel;
};


class XmlElement : public XmlNode
{
public:
    XmlElement (const char * in_value);

    XmlElement( const XmlElement& );

    XmlElement& operator=( const XmlElement& base );

    virtual ~XmlElement();

    const char* Attribute( const char* name ) const;

    const char* Attribute( const char* name, int* i ) const;

    const char* Attribute( const char* name, double* d ) const;

    int QueryIntAttribute( const char* name, int* _value ) const;
    
    int QueryUnsignedAttribute( const char* name, unsigned* _value ) const;

    int QueryBoolAttribute( const char* name, bool* _value ) const;
    
    int QueryDoubleAttribute( const char* name, double* _value ) const;
    
    int QueryFloatAttribute( const char* name, float* _value ) const
    {
        double d;
        int result = QueryDoubleAttribute( name, &d );
        if ( result == XML_SUCCESS ) {
            *_value = (float)d;
        }
        return result;
    }

    void SetAttribute( const char* name, const char * _value );

    void SetAttribute( const char * name, int value );

    void SetDoubleAttribute( const char * name, double value );

    void RemoveAttribute( const char * name );

    const XmlAttribute* FirstAttribute() const    { return attributeSet.First(); }
    XmlAttribute* FirstAttribute()                { return attributeSet.First(); }
    const XmlAttribute* LastAttribute() const     { return attributeSet.Last(); }
    XmlAttribute* LastAttribute()                 { return attributeSet.Last(); }

    const char* GetText() const;

    virtual XmlNode* Clone() const;
    
    virtual void Print( FILE* cfile, int depth ) const;

    virtual const char* Parse( const char* p, XmlParsingData* data, XmlEncoding encoding );

    virtual const XmlElement* ToElement() const { return this; }
    virtual XmlElement* ToElement() { return this; }

    virtual bool Accept( XmlVisitor* visitor ) const;

protected:
    void CopyTo( XmlElement* target ) const;
    void ClearThis();

    const char* ReadValue( const char* in, XmlParsingData* prevData, XmlEncoding encoding );

private:
    XmlAttributeSet attributeSet;
};


class XmlComment : public XmlNode
{
public:
    XmlComment() : XmlNode( XmlNode::XML_COMMENT ) {}

    XmlComment( const char* _value ) : XmlNode( XmlNode::XML_COMMENT ) {
        SetValue( _value );
    }
    XmlComment( const XmlComment& );
    XmlComment& operator=( const XmlComment& base );

    virtual ~XmlComment(){}

    virtual XmlNode* Clone() const;

    virtual void Print( FILE* cfile, int depth ) const;

    virtual const char* Parse( const char* p, XmlParsingData* data, XmlEncoding encoding );

    virtual const XmlComment*  ToComment() const { return this; }
    virtual XmlComment* ToComment() { return this; }

    virtual bool Accept( XmlVisitor* visitor ) const;

protected:
    void CopyTo( XmlComment* target ) const;
};


class XmlText : public XmlNode
{
    friend class XmlElement;

public:
    XmlText (const char * initValue ) : XmlNode (XmlNode::XML_TEXT)
    {
        SetValue( initValue );
        cdata = false;
    }
    virtual ~XmlText() {}

    XmlText( const XmlText& copy ) : XmlNode( XmlNode::XML_TEXT ) { copy.CopyTo( this ); }
    XmlText& operator=( const XmlText& base ) { base.CopyTo( this ); return *this; }

    virtual void Print( FILE* cfile, int depth ) const;

    bool CDATA() const { return cdata; }
    
    void SetCDATA( bool _cdata ) { cdata = _cdata; }

    virtual const char* Parse( const char* p, XmlParsingData* data, XmlEncoding encoding );

    virtual const XmlText* ToText() const { return this; } 
    virtual XmlText* ToText() { return this; } 

    virtual bool Accept( XmlVisitor* content ) const;

protected:
    virtual XmlNode* Clone() const;
    void CopyTo( XmlText* target ) const;

    bool Blank() const;

private:
    bool cdata;
};


class XmlDeclaration : public XmlNode
{
public:
    XmlDeclaration() : XmlNode( XmlNode::XML_DECLARATION ) {}

    XmlDeclaration( const char* _version,
                    const char* _encoding,
                    const char* _standalone );

    XmlDeclaration( const XmlDeclaration& copy );
    XmlDeclaration& operator=( const XmlDeclaration& copy );

    virtual ~XmlDeclaration(){}

    const char *Version() const { return version.c_str (); }

    const char *Encoding() const { return encoding.c_str (); }

    const char *Standalone() const { return standalone.c_str (); }

    virtual XmlNode* Clone() const;

    virtual void Print( FILE* cfile, int depth, XmlString* str ) const;
    virtual void Print( FILE* cfile, int depth ) const {
        Print( cfile, depth, 0 );
    }

    virtual const char* Parse( const char* p, XmlParsingData* data, XmlEncoding encoding );

    virtual const XmlDeclaration* ToDeclaration() const { return this; } 
    virtual XmlDeclaration* ToDeclaration() { return this; } 

    virtual bool Accept( XmlVisitor* visitor ) const;

protected:
    void CopyTo( XmlDeclaration* target ) const;

private:
    XmlString version;
    XmlString encoding;
    XmlString standalone;
};


class XmlUnknown : public XmlNode
{
public:
    XmlUnknown() : XmlNode( XmlNode::XML_UNKNOWN ){}
    virtual ~XmlUnknown() {}

    XmlUnknown( const XmlUnknown& copy ) : XmlNode( XmlNode::XML_UNKNOWN ) { copy.CopyTo( this ); }
    XmlUnknown& operator=( const XmlUnknown& copy ) { copy.CopyTo( this ); return *this; }


    virtual XmlNode* Clone() const;

    virtual void Print( FILE* cfile, int depth ) const;

    virtual const char* Parse( const char* p, XmlParsingData* data, XmlEncoding encoding );

    virtual const XmlUnknown* ToUnknown() const { return this; }
    virtual XmlUnknown* ToUnknown() { return this; }

    virtual bool Accept( XmlVisitor* content ) const;

protected:
    void CopyTo( XmlUnknown* target ) const;
};

class XmlDocument : public XmlNode
{
public:
    XmlDocument();
    XmlDocument( const char * documentName );
    XmlDocument( const XmlDocument& copy );
    XmlDocument& operator=( const XmlDocument& copy );

    virtual ~XmlDocument() {}

    bool LoadFile( XmlEncoding encoding = XML_DEFAULT_ENCODING );
    
    bool SaveFile() const;
    
    bool LoadFile( const char * filename, XmlEncoding encoding = XML_DEFAULT_ENCODING );
    
    bool SaveFile( const char * filename ) const;

    bool LoadFile( FILE*, XmlEncoding encoding = XML_DEFAULT_ENCODING );
    
    bool SaveFile( FILE* ) const;

    virtual const char* Parse( const char* p, XmlParsingData* data = 0, XmlEncoding encoding = XML_DEFAULT_ENCODING );

    const XmlElement* RootElement() const { return FirstChildElement(); }
    XmlElement* RootElement() { return FirstChildElement(); }

    bool Error() const { return error; }

    const char * ErrorDesc() const { return errorDesc.c_str (); }

    int ErrorId() const { return errorId; }

    int ErrorRow() const { return errorLocation.row+1; }
    int ErrorCol() const { return errorLocation.col+1; }

    void SetTabSize( int _tabsize ) { tabsize = _tabsize; }

    int TabSize() const    { return tabsize; }

    void ClearError()
    {
        error = false;
        errorId = 0;
        errorDesc = "";
        errorLocation.row = errorLocation.col = 0;
    }

    void Print() const { Print( stdout, 0 ); }

    virtual void Print( FILE* cfile, int depth = 0 ) const;

    void SetError( int err, const char* errorLocation, XmlParsingData* prevData, XmlEncoding encoding );

    virtual const XmlDocument* ToDocument() const { return this; }
    virtual XmlDocument* ToDocument() { return this; }

    virtual bool Accept( XmlVisitor* content ) const;

protected:
    virtual XmlNode* Clone() const;

private:
    void CopyTo( XmlDocument* target ) const;

    bool error;
    int  errorId;
    XmlString errorDesc;
    int tabsize;
    XmlCursor errorLocation;
    bool useMicrosoftBOM;
};

class XmlHandle
{
public:
    XmlHandle( XmlNode* _node ) { this->node = _node; }
    XmlHandle( const XmlHandle& ref ) { this->node = ref.node; }
    XmlHandle operator=( const XmlHandle& ref ) { if ( &ref != this ) this->node = ref.node; return *this; }

    XmlHandle FirstChild() const;
    
    XmlHandle FirstChild( const char * value ) const;
    
    XmlHandle FirstChildElement() const;
    
    XmlHandle FirstChildElement( const char * value ) const;

    XmlHandle Child( const char* value, int index ) const;

    XmlHandle Child( int index ) const;

    XmlHandle ChildElement( const char* value, int index ) const;

    XmlHandle ChildElement( int index ) const;

    XmlNode* ToNode() const { return node; }
    XmlElement* ToElement() const { return ( ( node && node->ToElement() ) ? node->ToElement() : 0 ); }
    XmlText* ToText() const { return ( ( node && node->ToText() ) ? node->ToText() : 0 ); }
    XmlUnknown* ToUnknown() const { return ( ( node && node->ToUnknown() ) ? node->ToUnknown() : 0 ); }

    XmlNode* Node() const { return ToNode(); }
    XmlElement* Element() const { return ToElement(); }
    XmlText* Text() const { return ToText(); }
    XmlUnknown* Unknown() const    { return ToUnknown(); }

private:
    XmlNode* node;
};


class XmlPrinter : public XmlVisitor
{
public:
    XmlPrinter() : depth( 0 ), simpleTextPrint( false ),
                     buffer(), indent( "    " ), lineBreak( "\n" ) {}

    virtual bool VisitEnter( const XmlDocument& doc );
    virtual bool VisitExit( const XmlDocument& doc );

    virtual bool VisitEnter( const XmlElement& element, const XmlAttribute* firstAttribute );
    virtual bool VisitExit( const XmlElement& element );

    virtual bool Visit( const XmlDeclaration& declaration );
    virtual bool Visit( const XmlText& text );
    virtual bool Visit( const XmlComment& comment );
    virtual bool Visit( const XmlUnknown& unknown );

    void SetIndent( const char* _indent ){ indent = _indent ? _indent : "" ; }

    const char* Indent(){ return indent.c_str(); }

    void SetLineBreak( const char* _lineBreak ){ lineBreak = _lineBreak ? _lineBreak : ""; }

    const char* LineBreak(){ return lineBreak.c_str(); }

    void SetStreamPrinting(){ indent = ""; lineBreak = ""; }

    const char* CStr() { return buffer.c_str(); }

    size_t Size() { return buffer.size(); }

private:
    void DoIndent(){
        for( int i=0; i<depth; ++i )
            buffer += indent;
    }
    void DoLineBreak() {
        buffer += lineBreak;
    }

    int depth;
    bool simpleTextPrint;
    XmlString buffer;
    XmlString indent;
    XmlString lineBreak;
};



}


#ifdef _MSC_VER
#pragma warning( pop )
#endif

#endif